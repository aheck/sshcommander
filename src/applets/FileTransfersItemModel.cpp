#include "FileTransfersItemModel.h"

QModelIndex FileTransfersItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex FileTransfersItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int FileTransfersItemModel::rowCount(const QModelIndex &parent) const
{
    if (this->connectionId.isEmpty()) {
        return 0;
    }

    return SSHConnectionManager::getInstance().countFileTransferJobs(this->connectionId);
}

int FileTransfersItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(FileTransferColumns::Count);
}

QVariant FileTransfersItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(FileTransferColumns::TransferType):
            return QVariant(tr("Transfer Type"));
        case static_cast<int>(FileTransferColumns::Source):
            return QVariant(tr("Source"));
        case static_cast<int>(FileTransferColumns::Destination):
            return QVariant(tr("Destination"));
        case static_cast<int>(FileTransferColumns::Transferred):
            return QVariant(tr("Transferred"));
        case static_cast<int>(FileTransferColumns::Speed):
            return QVariant(tr("Speed"));
        case static_cast<int>(FileTransferColumns::State):
            return QVariant(tr("Status"));
    }

    return QVariant();
}

QVariant FileTransfersItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    if (this->connectionId.isEmpty()) {
        return QVariant();
    }

    std::shared_ptr<FileTransferJob> job = SSHConnectionManager::getInstance().getFileTransferJob(connectionId, index.row());
    if (job == nullptr) {
        return QVariant();
    }

    QString extension;

    switch (index.column()) {
        case (static_cast<int>(FileTransferColumns::TransferType)):
            if (job->getType() == FileTransferType::Upload) {
                return QVariant("Upload");
            }

            return QVariant("Download");
        case (static_cast<int>(FileTransferColumns::Source)):
            return job->getSourceHostname() + ":" + job->getLabel();
        case (static_cast<int>(FileTransferColumns::Destination)):
            return job->getTargetHostname() + ":" + job->getTargetDir();
        case (static_cast<int>(FileTransferColumns::Transferred)):
            return Util::formatBytes(job->bytesTransferred);
        case (static_cast<int>(FileTransferColumns::Speed)):
            return Util::formatBytes(job->getBytesPerSecond()) + "/s";
        case (static_cast<int>(FileTransferColumns::State)):
            if (role == Qt::DecorationRole) {
                if (job->getState() == FileTransferState::Completed) {
                    return QIcon(":/images/green-light.svg");
                } else if (job->getState() == FileTransferState::FailedConnect
                        || job->getState() == FileTransferState::Failed
                        || job->getState() == FileTransferState::Canceled) {
                    return QIcon(":/images/red-light.svg");
                } else {
                    return QIcon(":/images/yellow-light.svg");
                }
            }

            return FileTransferJob::fileTransferStateToString(job->getState());
    }

    return QVariant();
}

Qt::ItemFlags FileTransfersItemModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

void FileTransfersItemModel::setConnectionId(const QString &connectionId)
{
    this->connectionId = connectionId;
}

void FileTransfersItemModel::reloadData()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void FileTransfersItemModel::jobDataChanged(QUuid jobUuid)
{
    int row = SSHConnectionManager::getInstance().getFileTransferJobRowByUuid(connectionId, jobUuid);

    if (row < 0) {
        return;
    }

    this->dataChanged(this->index(row, 0, QModelIndex()),
            this->index(row, this->columnCount(QModelIndex()), QModelIndex()));
}

bool FileTransfersItemModel::removeFileTransferJob(int row)
{
    if (this->connectionId.isEmpty()) {
        return false;
    }

    emit layoutAboutToBeChanged();

    SSHConnectionManager::getInstance().removeFileTransferJob(this->connectionId, row);

    emit layoutChanged();

    return true;
}
