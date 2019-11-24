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

private:
    int sshPort;
    static const QString testCFile;
    static const QString testCFileSHA1Sum;

    int stopDockerContainer();
};

#endif
