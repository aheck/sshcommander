#include "FileTransferTests.h"

const QString FileTransferTests::testCFileContent = "#include <stdio.h>\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    unsigned long test;\n"
        "    printf(\"test: %ld\\n\", sizeof(test));\n"
        "    return 0;\n"
        "}\n";

const QString FileTransferTests::testCFileSHA1Sum = "93f4ca7a4cb79b9e911881c8b8780756b5695c01";

const QString FileTransferTests::isoFilePath = QDir::homePath() + "/Downloads/TinyCore-10.1.iso";
const QString FileTransferTests::isoFileSHA1Sum = "e443d29775abab64322f8d0ef81dd11104553dac";

void FileTransferTests::init()
{
    // If the previous test run crashed the docker container might still be
    // running. Ensure that it doesn't interfere with this test run.
    this->stopDockerContainer();

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

void FileTransferTests::cleanup()
{
    QCOMPARE(this->stopDockerContainer(), 0);
}

int FileTransferTests::stopDockerContainer()
{
    QProcess proc;
    proc.start("docker stop testing_sshd");
    proc.waitForFinished(-1);

    return proc.exitCode();
}

void FileTransferTests::testSimpleDownload()
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = this->sshPort;

    QTemporaryDir tmpDir("");
    QString filename = tmpDir.path() + "/test.c";
    TestHelpers::writeStringToFile(filename, FileTransferTests::testCFileContent);

    QCOMPARE(TestHelpers::scpFiles(connEntry, filename), 0);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    job->addFileToCopy("/root/test.c");

    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    worker->connectWithThread(thread);

    qDebug() << "Starting file download";
    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::genFileChecksum(targetDir.path() + "/test.c", QCryptographicHash::Sha1);
    QCOMPARE(checksum, QString(FileTransferTests::testCFileSHA1Sum));
}

void FileTransferTests::testSimpleUpload()
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = this->sshPort;

    QTemporaryDir tmpDir("");
    QString filename = tmpDir.path() + "/test.c";
    TestHelpers::writeStringToFile(filename, FileTransferTests::testCFileContent);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Upload, "/root");
    job->addFileToCopy(filename);

    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    worker->connectWithThread(thread);

    qDebug() << "Starting file upload";
    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::sshSHA1Sum(connEntry, "/root/test.c");
    QCOMPARE(checksum, QString(FileTransferTests::testCFileSHA1Sum));
}

void FileTransferTests::testIsoFileDownload()
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = this->sshPort;

    QCOMPARE(TestHelpers::scpFiles(connEntry, FileTransferTests::isoFilePath), 0);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    job->addFileToCopy("/root/" + Util::basename(FileTransferTests::isoFilePath));

    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    worker->connectWithThread(thread);

    qDebug() << "Starting file download";
    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::genFileChecksum(targetDir.path() + "/" + Util::basename(FileTransferTests::isoFilePath), QCryptographicHash::Sha1);
    QCOMPARE(checksum, QString(FileTransferTests::isoFileSHA1Sum));
}

void FileTransferTests::testIsoFileUpload()
{
    auto connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->hostname = "localhost";
    connEntry->username = "root";
    connEntry->password = "root";
    connEntry->port = this->sshPort;

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Upload, "/root");
    job->addFileToCopy(FileTransferTests::isoFilePath);

    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    worker->connectWithThread(thread);

    qDebug() << "Starting file upload";
    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::sshSHA1Sum(connEntry, "/root/" + Util::basename(FileTransferTests::isoFilePath));
    QCOMPARE(checksum, QString(FileTransferTests::isoFileSHA1Sum));
}
