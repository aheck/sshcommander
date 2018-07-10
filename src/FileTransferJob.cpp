#include "FileTransferJob.h"

FileTransferJob::FileTransferJob(std::shared_ptr<SSHConnectionEntry> connEntry, FileTransferType type, QString targetDir)
{
    this->uuid = QUuid::createUuid();
    this->connEntry = connEntry;
    this->type = type;
    this->state = FileTransferState::Preparing;
    this->targetDir = targetDir;
    this->thread = nullptr;
    this->bytesPerSecond = 0;
    this->bytesTransferred = 0;
    this->cancelationRequested = false;
}

QUuid FileTransferJob::getUuid() const
{
    return this->uuid;
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
    emit dataChanged(this->getUuid());
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

void FileTransferJob::setErrorMessage(QString message)
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

QString FileTransferJob::fileTransferStateToString(FileTransferState value)
{
    static QStringList strings = {
        "Preparing",
        "Connecting",
        "In Progress",
        "Failed Connect",
        "Failed",
        "Canceled",
        "Completed"
    };

    return strings.at(static_cast<int>(value));
}

QString FileTransferJob::getSourceHostname()
{
    if (this->type == FileTransferType::Upload) {
        return QHostInfo::localHostName();
    }

    return this->connEntry->hostname;
}

QString FileTransferJob::getTargetHostname()
{
    if (this->type == FileTransferType::Upload) {
        return this->connEntry->hostname;
    }

    return QHostInfo::localHostName();
}

void FileTransferJob::setBytesPerSecond(uint64_t bytesPerSecond)
{
    this->bytesPerSecond = bytesPerSecond;

    emit dataChanged(this->getUuid());
}

uint64_t FileTransferJob::getBytesPerSecond()
{
    return this->bytesPerSecond;
}
