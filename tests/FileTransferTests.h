#ifndef FILETRANSFERTESTS_H
#define FILETRANSFERTESTS_H

#include <QObject>

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTextStream>
#include <QtTest/QtTest>

#include <memory>
#include <iostream>

#include "FileTransferJob.h"
#include "FileTransferWorker.h"
#include "TestHelpers.h"

class FileTransferTests : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testSimpleDownload();
    void testSimpleUpload();
    void testIsoFileDownload();
    void testIsoFileUpload();
    void testDirDownload();
    void testDirUpload();

private:
    int sshPort;
    static const QString testCFileContent;
    static const QString testCFileSHA1Sum;
    static const QString isoFilePath;
    static const QString isoFileSHA1Sum;

    int stopDockerContainer();
    void runFileTransferJob(std::shared_ptr<SSHConnectionEntry> connEntry, std::shared_ptr<FileTransferJob> job);
    bool createTestDirTree(const QString &rootDir);
};

#endif
