#include "SSHConnectionItemModel.h"

QModelIndex SSHConnectionItemModel::index(int row, int column, const QModelIndex &parent = QModelIndex()) const
{
    return this->createIndex(row, column);
}

QModelIndex SSHConnectionItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SSHConnectionItemModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    return this->entries.size();
}

int SSHConnectionItemModel::columnCount(const QModelIndex &parent = QModelIndex()) const
{
    return 1;
}

QVariant SSHConnectionItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    std::shared_ptr<SSHConnectionEntry> entry = this->entries.at(index.row());

    return QVariant(entry->name);
}

void SSHConnectionItemModel::appendConnectionEntry(std::shared_ptr<SSHConnectionEntry> entry)
{
    int pos = this->rowCount();
    this->beginInsertRows(QModelIndex(), pos, pos);
    this->entries << entry;
    this->sshConnByHost[entry->name] = entry;
    this->endInsertRows();
}

void SSHConnectionItemModel::removeConnectionEntry(std::shared_ptr<SSHConnectionEntry> entry)
{
    QModelIndex index = this->getIndexForSSHConnectionEntry(entry);

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    this->entries.removeAt(index.row());
    this->sshConnByHost.remove(entry->name);
    endRemoveRows();
}

std::shared_ptr<SSHConnectionEntry> SSHConnectionItemModel::getConnEntry(int index)
{
    if (index < 0) {
        return nullptr;
    }

    if (index > (this->entries.size() - 1)) {
        return nullptr;
    }

    return this->entries.at(index);
}

std::shared_ptr<SSHConnectionEntry> SSHConnectionItemModel::getConnEntryByName(const QString name)
{
    return this->sshConnByHost[name];
}

QModelIndex SSHConnectionItemModel::getIndexForSSHConnectionEntry(const std::shared_ptr<SSHConnectionEntry> entry) const
{
    if (entry == nullptr) {
        return QModelIndex();
    }

    for (int i = 0; i < this->entries.size(); i++) {
        if (this->entries.at(i) == entry) {
            return this->index(i, 0);
        }
    }

    return QModelIndex();
}
