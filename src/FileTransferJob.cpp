#include "FileTransferJob.h"

FileTransferJob::FileTransferJob(std::shared_ptr<SSHConnectionEntry> connEntry, FileTransferType type, QString targetDir)
{
    this->connEntry = connEntry;
    this->type = type;
    this->state = FileTransferState::Preparing;
    this->targetDir = targetDir;
    this->thread = nullptr;
    this->bytesPerSecond = 0;
    this->bytesTransferred = 0;
}

std::shared_ptr<SSHConnectionEntry> FileTransferJob::getConnEntry() const
{
    return this->connEntry;
}

void FileTransferJob::setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;
}

FileTransferType FileTransferJob::getType() const
{
    return this->type;
}

FileTransferState FileTransferJob::getState() const
{
    return this->state;
}

void FileTransferJob::setState(FileTransferState state)
{
    this->state = state;
}

void FileTransferJob::addFileToCopy(QString filename)
{
    this->filesToCopy.append(filename);
}

QString FileTransferJob::getTargetDir() const
{
    return this->targetDir;
}

QStringList FileTransferJob::getFilesToCopy() const
{
    return this->filesToCopy;
}

QString FileTransferJob::setErrorMessage(QString message)
{
    this->errorMessage = message;
}

QString FileTransferJob::getErrorMessage() const
{
    return this->errorMessage;
}

QThread* FileTransferJob::getThread() const
{
    return this->thread;
}

void FileTransferJob::setThread(QThread *thread)
{
    this->thread = thread;
}

uint64_t FileTransferJob::getBytesPerSecond() const
{
    return this->bytesPerSecond;
}

uint64_t FileTransferJob::getBytesTransferred() const
{
    return this->bytesTransferred;
}
