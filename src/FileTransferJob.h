/*****************************************************************************
 *
 * FileTransferJob represents a single file transfer.
 *
 * A file transfer job can be a download or an upload and it can be
 * responsible for transfering an arbitrary number of files and directory
 * trees.
 *
 ****************************************************************************/

#ifndef FILETRANSFERJOB_H
#define FILETRANSFERJOB_H

#include <atomic>

#include <QHostInfo>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QUuid>

#include "SSHConnectionEntry.h"

enum class FileTransferType : unsigned char
{
    Download = 0,
    Upload
};

enum class FileTransferState : unsigned char
{
    Preparing = 0,
    Connecting,
    InProgress,
    FailedConnect,
    Failed,
    Canceled,
    Completed
};

class FileTransferJob : public QObject
{
    Q_OBJECT

public:
    FileTransferJob(std::shared_ptr<SSHConnectionEntry> connEntry, FileTransferType type, QString targetDir);

    QUuid getUuid() const;
    std::shared_ptr<SSHConnectionEntry> getConnEntry() const;
    void setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);
    FileTransferType getType() const;
    FileTransferState getState() const;
    void setState(FileTransferState state);
    void addFileToCopy(QString filename);
    QString getTargetDir() const;
    QStringList getFilesToCopy() const;
    void setErrorMessage(QString message);
    QString getErrorMessage() const;
    QThread* getThread() const;
    void setThread(QThread *thread);
    QString getSourceHostname();
    QString getTargetHostname();
    void setBytesPerSecond(uint64_t bytesPerSecond);
    uint64_t getBytesPerSecond();

    std::atomic<bool> cancelationRequested;
    std::atomic<uint64_t> bytesTransferred;

    static QString fileTransferStateToString(FileTransferState value);

signals:
    void dataChanged(QUuid uuid);

private:
    QUuid uuid;
    std::shared_ptr<SSHConnectionEntry> connEntry;
    FileTransferType type;
    std::atomic<FileTransferState> state;
    QString targetDir;

    // list of files and directories to transfer
    QStringList filesToCopy;

    QString errorMessage;
    QThread *thread;

    std::atomic<uint64_t> bytesPerSecond;
};

#endif
