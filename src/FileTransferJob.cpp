#include "FileTransferJob.h"

FileTransferJob::FileTransferJob(FileTransferType type)
{
    this->type = type;
    this->state = FileTransferState::New;
}

FileTransferType FileTransferJob::getType() const
{
    return this->type;
}

FileTransferState FileTransferJob::getState() const
{
    return this->state;
}

QString FileTransferJob::getTargetDirectory() const
{
    return this->targetDirectory;
}

QStringList FileTransferJob::getFilesToCopy() const
{
    return this->filesToCopy;
}

QString FileTransferJob::getErrorMessage() const
{
    return this->errorMessage;
}

QThread* FileTransferJob::getThread() const
{
    return this->thread;
}

uint64_t FileTransferJob::getBytesPerSecond() const
{
    return this->bytesPerSecond;
}

uint64_t FileTransferJob::getBytesTransferred() const
{
    return this->bytesTransferred;
}
