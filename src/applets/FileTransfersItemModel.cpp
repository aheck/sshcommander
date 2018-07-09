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
            return QVariant(tr("State"));
    }

    return QVariant();
}

QVariant FileTransfersItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole) {
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
            if (job->getFilesToCopy().length() > 1) {
                extension = ", ...";
            }

            return QVariant(job->getSourceHostname() + ":" + job->getFilesToCopy().first() + extension);
        case (static_cast<int>(FileTransferColumns::Destination)):
            return QVariant(job->getTargetHostname() + ":" + job->getTargetDir());
        case (static_cast<int>(FileTransferColumns::Transferred)):
            return QVariant(Util::formatBytes(job->bytesTransferred));
        case (static_cast<int>(FileTransferColumns::Speed)):
            return QVariant(Util::formatBytes(job->bytesPerSecond) + "/s");
        case (static_cast<int>(FileTransferColumns::State)):
            return QVariant(FileTransferJob::fileTransferStateToString(job->getState()));
            /*
        case (static_cast<int>(FileTransferColumns::Connected)):
            if (role== Qt::DecorationRole) {
                if (tunnel->isConnected()) {
                    return QIcon(":/images/green-light.svg");
                } else {
                    return QIcon(":/images/red-light.svg");
                }
            }

            return QVariant("");
            */
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
