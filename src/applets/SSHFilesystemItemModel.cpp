#include "SSHFilesystemItemModel.h"

QModelIndex SSHFilesystemItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex SSHFilesystemItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SSHFilesystemItemModel::rowCount(const QModelIndex &parent) const
{
    if (this->username == "" || this->hostname == "") {
        return 0;
    }

    return SSHFilesystemManager::getInstance().countMounts(this->username, this->hostname);
}

int SSHFilesystemItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(SSHFSColumns::Count);
}

QVariant SSHFilesystemItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(SSHFSColumns::LocalDir):
            return QVariant(tr("Local Directory"));
        case static_cast<int>(SSHFSColumns::RemoteDir):
            return QVariant(tr("Remote Directory"));
        case static_cast<int>(SSHFSColumns::ShortDescription):
                return QVariant(tr("Short Description"));
        case static_cast<int>(SSHFSColumns::Mounted):
            return QVariant(tr("Mounted"));
    }

    return QVariant();
}

QVariant SSHFilesystemItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    std::shared_ptr<SSHFSMountEntry> mountEntry = SSHFilesystemManager::getInstance().getMountEntry(this->username, this->hostname, index.row());

    if (mountEntry == nullptr) {
        return QVariant("");
    }

    switch (index.column()) {
        case (static_cast<int>(SSHFSColumns::LocalDir)):
            return QVariant(mountEntry->localDir);
        case (static_cast<int>(SSHFSColumns::RemoteDir)):
            return QVariant(mountEntry->remoteDir);
        case (static_cast<int>(SSHFSColumns::ShortDescription)):
            return QVariant(mountEntry->shortDescription);
        case (static_cast<int>(SSHFSColumns::Mounted)):
            if (role== Qt::DecorationRole) {
                if (mountEntry->isMounted()) {
                    return QIcon(":/images/green-light.svg");
                } else {
                    return QIcon(":/images/red-light.svg");
                }
            }

            return QVariant("");
    }

    return QVariant();
}

void SSHFilesystemItemModel::clear()
{
    emit layoutAboutToBeChanged();

    emit layoutChanged();
}

void SSHFilesystemItemModel::updateData(QString data)
{
}

void SSHFilesystemItemModel::sort(int column, Qt::SortOrder order)
{
}

void SSHFilesystemItemModel::setConnectionStrings(QString username, QString hostname)
{
    beginResetModel();

    this->username = username;
    this->hostname = hostname;

    endResetModel();
}

void SSHFilesystemItemModel::reloadData()
{
    this->beginResetModel();
    this->endResetModel();
}
