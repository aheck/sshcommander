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
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QTemporaryDir tmpDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(tmpDir.isValid());
    QString filename = tmpDir.path() + "/test.c";
    TestHelpers::writeStringToFile(filename, FileTransferTests::testCFileContent);

    QCOMPARE(TestHelpers::scpFiles(connEntry, filename), 0);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(targetDir.isValid());
    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    job->addFileToCopy("/root/test.c");

    qDebug() << "Starting file download";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::genFileChecksum(targetDir.path() + "/test.c", QCryptographicHash::Sha1);
    QCOMPARE(checksum, QString(FileTransferTests::testCFileSHA1Sum));
}

void FileTransferTests::testSimpleUpload()
{
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QTemporaryDir tmpDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(tmpDir.isValid());
    QString filename = tmpDir.path() + "/test.c";
    TestHelpers::writeStringToFile(filename, FileTransferTests::testCFileContent);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Upload, "/root");
    job->addFileToCopy(filename);

    qDebug() << "Starting file upload";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::sshSHA1Sum(connEntry, "/root/test.c");
    QCOMPARE(checksum, QString(FileTransferTests::testCFileSHA1Sum));
}

void FileTransferTests::testIsoFileDownload()
{
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QCOMPARE(TestHelpers::scpFiles(connEntry, FileTransferTests::isoFilePath), 0);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(targetDir.isValid());
    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    job->addFileToCopy("/root/" + Util::basename(FileTransferTests::isoFilePath));

    qDebug() << "Starting file download";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::genFileChecksum(targetDir.path() + "/" + Util::basename(FileTransferTests::isoFilePath), QCryptographicHash::Sha1);
    QCOMPARE(checksum, QString(FileTransferTests::isoFileSHA1Sum));
}

void FileTransferTests::testIsoFileUpload()
{
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Upload, "/root");
    job->addFileToCopy(FileTransferTests::isoFilePath);

    qDebug() << "Starting file upload";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QString checksum = TestHelpers::sshSHA1Sum(connEntry, "/root/" + Util::basename(FileTransferTests::isoFilePath));
    QCOMPARE(checksum, QString(FileTransferTests::isoFileSHA1Sum));
}

void FileTransferTests::testDirDownload()
{
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QTemporaryDir tmpDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(tmpDir.isValid());
    QVERIFY(this->createTestDirTree(tmpDir.path()));
    QVERIFY(!QDir(tmpDir.path()).isEmpty());
    QCOMPARE(TestHelpers::scpFiles(connEntry, tmpDir.path()), 0);

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    QTemporaryDir targetDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(targetDir.isValid());

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Download, targetDir.path());
    QString remoteDir = "/root/" + Util::basename(tmpDir.path());
    job->addFileToCopy(remoteDir);

    qDebug() << "Starting dir download";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QVERIFY(TestHelpers::sshCompareDirs(connEntry, targetDir.path() + "/" + Util::basename(tmpDir.path()), remoteDir));
}

void FileTransferTests::testDirUpload()
{
    auto connEntry = TestHelpers::buildConnEntry(this->sshPort);

    QTemporaryDir tmpDir("/tmp/qtest-filetransfer-XXXXXX");
    QVERIFY(tmpDir.isValid());
    QVERIFY(this->createTestDirTree(tmpDir.path()));
    QVERIFY(!QDir(tmpDir.path()).isEmpty());

    QVERIFY(TestHelpers::connectConnEntry(connEntry));

    auto job = std::make_shared<FileTransferJob>(connEntry, FileTransferType::Upload, "/root");
    QString remoteDir = "/root/" + Util::basename(tmpDir.path());
    job->addFileToCopy(tmpDir.path());

    qDebug() << "Starting dir upload";
    this->runFileTransferJob(connEntry, job);

    QCOMPARE(job->getState(), FileTransferState::Completed);

    QVERIFY(TestHelpers::sshCompareDirs(connEntry, tmpDir.path(), remoteDir));
}

void FileTransferTests::runFileTransferJob(std::shared_ptr<SSHConnectionEntry> connEntry, std::shared_ptr<FileTransferJob> job)
{
    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->conn = connEntry->connection;
    worker->moveToThread(thread);

    worker->connectWithThread(thread);

    thread->start();

    while (thread->isRunning()) {
        QTest::qWait(200);
        QCoreApplication::processEvents();
    }
}

bool FileTransferTests::createTestDirTree(const QString &rootDir)
{
    QStringList paths;
    paths << "etc" << "usr/bin" << "usr/lib" << "home/testuser/Documents/Taxes"
        << "home/testuser/Documents/Letters" << "home/testuser2/Desktop"
        << "home/testuser2/Pictures";

    QStringList filesToCopy;
    filesToCopy << "/etc/fstab" << "/usr/bin/awk" << "/usr/bin/perl" << "/usr/bin/locale" << "/usr/bin/xargs";

    QList<QPair<QString, QString>> filesToCreate;
    filesToCreate.append(qMakePair(QString("/home/testuser/test.txt"), QString("Test Test Test")));
    filesToCreate.append(qMakePair(QString("/home/testuser/test2.txt"), QString("Test2 Test2 Test2")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Taxes/taxes2017.txt"), QString("Taxes 2017...")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Taxes/taxes2018.txt"), QString("Taxes 2018...")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Taxes/taxes2019.txt"), QString("Taxes 2019...")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Letters/letter1.txt"), QString("Letter 1...")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Letters/letter2.txt"), QString("Letter 2...")));
    filesToCreate.append(qMakePair(QString("/home/testuser/Documents/Letters/letter3.txt"), QString("Letter 3...")));

    QDir root(rootDir);

    // create all subdirectories first
    for (const QString &path : paths) {
        if (!root.mkpath(path)) {
            qDebug() << "Failed to create path: " << path;
            return false;
        }
    }

    // create all files
    for (const QString &filename : filesToCopy) {
        if (!QFile::copy(filename, rootDir + filename)) {
            qDebug() << "Failed to copy file: " << filename;
            return false;
        }
    }

    for (const QPair<QString, QString> data : filesToCreate) {
        QString filename = data.first;
        QString content = data.second;
        if (!TestHelpers::writeStringToFile(rootDir + filename, content)) {
            qDebug() << "Failed to create file: " << filename;
            return false;
        }
    }

    return true;
}
