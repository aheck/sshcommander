#include "FileTransferTests.h"

void FileTransferTests::initTestCase()
{
    QProcess proc1;
    proc1.start("docker run --rm -d -P --name testing_sshd rastasheep/ubuntu-sshd:18.04");
    proc1.waitForFinished(-1);

    QVERIFY(proc1.state() != QProcess::Running);
    QCOMPARE(proc1.exitCode(), 0);

    QProcess proc2;
    proc2.setProcessChannelMode(QProcess::MergedChannels);
    proc2.start("docker port testing_sshd 22");
    proc2.waitForFinished(-1);

    QCOMPARE(proc2.exitCode(), 0);
    QByteArray outBytes = proc2.readAllStandardOutput();
    QString output = QString::fromUtf8(outBytes);
    output = output.trimmed();
    QStringList components = output.split(":");
    bool success = false;
    this->sshPort = components[1].toInt(&success);
    QVERIFY(success);
}

void FileTransferTests::cleanupTestCase()
{
    QProcess proc;
    proc.start("docker stop testing_sshd");
    proc.waitForFinished(-1);

    QCOMPARE(proc.exitCode(), 0);
}

void FileTransferTests::testSimpleDownload()
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = this->sshPort;

    QString code = "#include <stdio.h>\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    unsigned long test;\n"
        "    printf(\"test: %ld\\n\", sizeof(test));\n"
        "    return 0;\n"
        "}\n";

    QTemporaryDir tmpDir("");
    QString filename = tmpDir.path() + "/test.c";
    TestHelpers::writeStringToFile(filename, code);

    QCOMPARE(this->scpFiles(connEntry, filename), 0);

    QVERIFY(this->connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    job->addFileToCopy("/root/test.c");

    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    thread->connect(thread, SIGNAL(started()), worker, SLOT(process()));
    thread->connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    thread->connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    thread->connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    qDebug() << "Starting file transfer";
    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checkSum = TestHelpers::genFileChecksum(targetDir.path() + "/test.c", QCryptographicHash::Sha1);
    QCOMPARE(checkSum, QString("93f4ca7a4cb79b9e911881c8b8780756b5695c01"));
}

void FileTransferTests::testSimpleUpload()
{

}

int FileTransferTests::scpFiles(std::shared_ptr<SSHConnectionEntry> connEntry, QString path)
{
    //QString cmd("/usr/bin/scp -o GlobalKnownHostsFile=/dev/null -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no ");
    PseudoTerminal term;
    QStringList args = QStringList() << "-o" << "GlobalKnownHostsFile=/dev/null" << "-o"
        << "UserKnownHostsFile=/dev/null" << "-o" << "StrictHostKeyChecking=no"
        << "-P" << QString::number(this->sshPort) << "-r" << path << "root@localhost:/root";
    term.start("/usr/bin/scp", args);

    std::cout << "OUTPUT\n";
    while (term.isRunning()) {
        std::cout << "In loop\n";
        if (!term.waitForReadyRead()) {
            continue;
        }

        QString output = term.readAllOutput();
        output = output.trimmed();
        std::cout << output.toStdString() << "\n";
        if (output.endsWith("password:")) {
            QString input = connEntry->password + "\n";
            term.sendData(input.toUtf8().constData());
            break;
        }
    }

    std::cout << "waitForFinished in\n";
    term.waitForFinished(-1);
    std::cout << "waitForFinished out\n";

    std::cout << "Status code: " << term.statusCode() << "\n";

    return term.statusCode();
}

bool FileTransferTests::connectConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    LIBSSH2_SESSION *session = nullptr;
    struct sockaddr_in sin;
    int sock;
    int retval;

    auto conn = std::make_shared<SSHConnection>();

    QHostInfo hostInfo = QHostInfo::fromName(connEntry->hostname);
    if (hostInfo.addresses().count() == 0) {
        return false;
    }

    QHostAddress address = hostInfo.addresses().first();

    qDebug() << "Trying to connect to " << address.toString() << ":" << connEntry->port;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(connEntry->port);
    sin.sin_addr.s_addr = htonl(address.toIPv4Address());
    if (::connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        qDebug() << "Failed to connect to " << connEntry->hostname
            << "(" << address.toString() << ")";
        return false;
    }

    conn->socket_fd = sock;

    qDebug() << "Connected socket";
 
    qDebug() << "Trying to init session";
    session = libssh2_session_init();

    if (!session) {
        return false;
    }

    qDebug() << "Succeeded to init session";
 
    qDebug() << "Trying handshake";
    while ((retval = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN);
    qDebug() << "Handshake succeeded";

    if (retval) {
        qDebug() << "Failed to establish SSH session: " <<  retval;
        return false;
    }

    QByteArray username = connEntry->username.toLatin1();

    if (!connEntry->password.isEmpty()) {
        QByteArray password;
        password = connEntry->password.toLatin1();

        retval = libssh2_userauth_password(session, username.data(), password.data());
        if (retval != 0) {
            qDebug() << "Authentication by password failed.";
            return false;
        }
    } else {
        QString sshkey = connEntry->sshkey;
        if (sshkey.isEmpty()) {
            sshkey = QDir(QDir::home().filePath(".ssh")).filePath("id_rsa");
        }
        QByteArray privateCertPath = sshkey.toLatin1();
        retval = libssh2_userauth_publickey_fromfile(session, username.data(), NULL, privateCertPath.data(), "");
        if (retval) {
            qDebug() << "Authentication by public key failed";
            return false;
        }
    }

    libssh2_session_set_blocking(session, 0);

    conn->session = session;
    conn->alive = true;
    connEntry->connection = conn;
    qDebug() << "Successfully connected";

    return true;
}
