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

#include <sys/stat.h>

#include <atomic>
#include <memory>

#include <QDir>
#include <QException>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QMutex>
#include <QString>
#include <QWaitCondition>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "FileTransferJob.h"
#include "SSHConnection.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionManager.h"
#include "Util.h"

enum class FileOverwriteAnswer : unsigned char
{
    None = 0,
    Yes,
    No,
    YesToAll,
    NoToAll
};

class FileTransferException : public QException
{
public:
    FileTransferException(QString const& message);

    void raise() const;
    FileTransferException *clone() const;
    QString getMessage() const;

private:
    QString message;
};

class FileTransferWorker : public QObject {
    Q_OBJECT

public:
    FileTransferWorker(std::shared_ptr<FileTransferJob> job);
    ~FileTransferWorker();

    void copyFileFromRemoteRecursively(QString remotePath, QString localDir);
    void copyFileToRemoteRecursively(QString localPath, QString remoteDir);
    void copyFileFromRemote(QString remotePath, QString localDir);
    void copyFileToRemote(QString localPath, QString remoteDir);
    void setFileOverwriteAnswer(FileOverwriteAnswer answer);
    void setFileOverwriteAnswerAndNotify(FileOverwriteAnswer answer);
    FileOverwriteAnswer getFileOverwriteAnswer();
public slots:
    void process();
signals:
    void finished();

    void askToOverwriteFile(QString title, QString message, QString infoText);
private:
    void waitUntilFileOverwriteAnswerChanged(QString title, QString message, QString infoText);

    std::shared_ptr<FileTransferJob> job;
    std::shared_ptr<SSHConnection> conn;

    // buffer for libssh2_sftp_realpath
    char buffer[1024 * 4];

    std::atomic<FileOverwriteAnswer> fileOverwriteAnswer;
    QWaitCondition fileOverwriteAnswerConditionVar;
    QMutex mutex;
};

#endif
