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
#include "PseudoTerminal.h"
#include "TestHelpers.h"

class FileTransferTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSimpleDownload();
    void testSimpleUpload();

private:
    int sshPort;

    int scpFiles(std::shared_ptr<SSHConnectionEntry> connEntry, QString path);
    bool connectConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);
};

#endif
