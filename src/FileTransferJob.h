#ifndef FILETRANSFERJOB_H
#define FILETRANSFERJOB_H

#include <QString>
#include <QStringList>
#include <QThread>

enum class FileTransferType : unsigned char
{
    Download = 0,
    Upload
};

enum class FileTransferState : unsigned char
{
    New = 0,
    Preparing,
    Running,
    Failed,
    Canceled,
    Succeeded
};

class FileTransferJob
{
public:
    FileTransferJob(FileTransferType type);

    FileTransferType getType() const;
    FileTransferState getState() const;
    QString getTargetDirectory() const;
    QStringList getFilesToCopy() const;
    QString getErrorMessage() const;
    QThread* getThread() const;
    uint64_t getBytesPerSecond() const;
    uint64_t getBytesTransferred() const;

private:
    FileTransferType type;
    FileTransferState state;
    QString targetDirectory;

    // list of files and directories to transfer
    QStringList filesToCopy;

    QString errorMessage;
    QThread *thread;
    uint64_t bytesPerSecond;
    uint64_t bytesTransferred;
};

#endif
