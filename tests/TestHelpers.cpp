#include "TestHelpers.h"

bool TestHelpers::writeStringToFile(const QString &filename, const QString &data)
{
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream stream(&file);
    stream << (data);

    file.close();

    return true;
}

QString TestHelpers::readFileContents(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }

    QTextStream stream(&file);

    return stream.readAll();
}

QString TestHelpers::genFileChecksum(const QString &filename, QCryptographicHash::Algorithm algo)
{
    QFile f(filename);

    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(algo);
        if (hash.addData(&f)) {
            return hash.result().toHex();
        }
    }

    return "";
}

std::shared_ptr<SSHConnectionEntry> TestHelpers::buildConnEntry(int sshPort)
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = sshPort;

    return connEntry;
}

bool TestHelpers::connectConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
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

int TestHelpers::scpFiles(std::shared_ptr<SSHConnectionEntry> connEntry, const QString &path)
{
    PseudoTerminal term;
    QStringList args = QStringList() << "-o" << "GlobalKnownHostsFile=/dev/null" << "-o"
        << "UserKnownHostsFile=/dev/null" << "-o" << "StrictHostKeyChecking=no"
        << "-P" << QString::number(connEntry->port) << "-r" << path << "root@localhost:/root";
    term.start("/usr/bin/scp", args);

    if (!enterPassword(term, connEntry->password)) {
        term.terminate();
        return 1;
    }

    term.waitForFinished(-1);

    return term.statusCode();
}

QString TestHelpers::sshSHA1Sum(std::shared_ptr<SSHConnectionEntry> connEntry, const QString &path)
{
    PseudoTerminal term;
    sshExecuteCommand(connEntry, term, "sha1sum " + path);

    QString output = term.readAllOutput();
    output = output.trimmed();
    QStringList fields = output.split(QRegExp("\\s+"));
    output = fields[0];

    return output;
}

int TestHelpers::sshInstallRsync(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    PseudoTerminal term;
    return sshExecuteCommand(connEntry, term, "apt update && apt -y install rsync");
}

int TestHelpers::sshExecuteCommand(std::shared_ptr<SSHConnectionEntry> connEntry, PseudoTerminal &term, const QString &cmd)
{
    QStringList args = QStringList() << "-o" << "GlobalKnownHostsFile=/dev/null" << "-o"
        << "UserKnownHostsFile=/dev/null" << "-o" << "StrictHostKeyChecking=no"
        << "-p" << QString::number(connEntry->port) << "root@localhost"
        << cmd;
    term.start("/usr/bin/ssh", args);

    if (!enterPassword(term, connEntry->password)) {
        term.terminate();
        return 1;
    }

    term.waitForFinished(-1);

    return term.statusCode();
}

bool TestHelpers::sshCompareDirs(std::shared_ptr<SSHConnectionEntry> connEntry, const QString &localDir, const QString &remoteDir)
{
    TestHelpers::sshInstallRsync(connEntry);

    PseudoTerminal term;
    QStringList args = QStringList() << "-e"
        << "ssh -o GlobalKnownHostsFile=/dev/null -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -p"
        + QString::number(connEntry->port) << "--dry-run" << "-vrc" << "--delete" << localDir + "/" << QString("root@localhost:") + remoteDir + "/";

    term.start("/usr/bin/rsync", args);

    if (!enterPassword(term, connEntry->password)) {
        term.terminate();
        return false;
    }

    term.waitForFinished(-1);

    QString output = term.readAllOutput();
    output = output.trimmed();
    QStringList lines = output.split(QRegExp("\n|\r\n|\r"));

    return lines.length() == 4; //empty response with no changed files contains 4 lines
}

bool TestHelpers::enterPassword(PseudoTerminal &term, const QString &password, int timeoutMsecs)
{
    while (term.isRunning()) {
        if (!term.waitForReadyRead(timeoutMsecs)) {
            break;
        }

        QString output = term.readAllOutput();
        output = output.trimmed();
        if (output.endsWith("password:")) {
            QString input = password + "\n";
            term.sendData(input.toUtf8().constData());
            return true;
        }
    }

    return false;
}
