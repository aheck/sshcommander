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
#include <QElapsedTimer>
#include <QException>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QMutex>
#include <QString>
#include <QWaitCondition>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "CResourceDestructor.h"
#include "FileTransferJob.h"
#include "SSHConnection.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionManager.h"
#include "Util.h"

#define CHECK_CANCEL() if (this->job->cancelationRequested) {throw FileTransferCancelException();}
#define TRANSFER_BUFFER_SIZE 1024 * 64

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

class FileTransferCancelException : public QException
{
    void raise() const {throw *this;};
};

class FileTransferWorker : public QObject {
    Q_OBJECT

public:
    FileTransferWorker(std::shared_ptr<FileTransferJob> job);
    ~FileTransferWorker();

    friend class FileTransferTests;

    void copyFileFromRemoteRecursively(QString remotePath, QString localDir);
    void copyFileToRemoteRecursively(QString localPath, QString remoteDir);
    void copyFileFromRemote(QString remotePath, QString localDir);
    void copyFileToRemote(QString localPath, QString remoteDir);
    void setFileOverwriteAnswer(FileOverwriteAnswer answer);
    void setFileOverwriteAnswerAndNotify(FileOverwriteAnswer answer);
    FileOverwriteAnswer getFileOverwriteAnswer();
    void connectWithThread(QThread *thread);
public slots:
    void process();
signals:
    void finished();

    void askToOverwriteFile(QString title, QString message, QString infoText);
private:
    void waitUntilFileOverwriteAnswerChanged(QString title, QString message, QString infoText);
    bool sftpFileExists(QString filename);
    void updateTransferSpeed(uint64_t bytediff);

    std::shared_ptr<FileTransferJob> job;
    std::shared_ptr<SSHConnection> conn;

    const int sleeptime = 20;

    // buffer for libssh2_sftp_realpath
    char realPathBuffer[1024 * 4];
    char transferBuffer[TRANSFER_BUFFER_SIZE];

    std::atomic<FileOverwriteAnswer> fileOverwriteAnswer;
    QWaitCondition fileOverwriteAnswerConditionVar;
    QMutex mutex;

    QElapsedTimer transferTimer; // used to calculate download speed
    qint64 lastTransferTime;
    qint64 accumulatedByteDiff;
};

#endif
