/*****************************************************************************
 *
 * FileTransferWorker implements the actual transfer of all files specified
 * by a FileTransferJob.
 *
 * It is a worker class for QThread. Each FileTransferWorker runs in a
 * separate thread and uses a separate connection created specifically for
 * executing its FileTransferJob.
 *
 ****************************************************************************/

#ifndef FILETRANSFERWORKER_H
#define FILETRANSFERWORKER_H

#include <memory>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "FileTransferJob.h"
#include "SSHConnection.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionManager.h"
#include "Util.h"

class FileTransferWorker : public QObject {
    Q_OBJECT

public:
    FileTransferWorker(std::shared_ptr<FileTransferJob> job);
    ~FileTransferWorker();

    void copyFileFromRemoteRecursively(QString remotePath, QString localDir);
    void copyFileToRemoteRecursively(QString localPath, QString remoteDir);
    void copyFileFromRemote(QString remotePath, QString localDir);
    void copyFileToRemote(QString localPath, QString remoteDir);
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
private:
    std::shared_ptr<FileTransferJob> job;
    std::shared_ptr<SSHConnection> conn;

    // buffer for libssh2_sftp_realpath
    char buffer[1024 * 4];
};

#endif
