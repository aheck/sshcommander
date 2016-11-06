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

    SSHConnectionEntry *entry = this->entries.at(index.row());

    return QVariant(entry->name);
}

void SSHConnectionItemModel::appendConnectionEntry(SSHConnectionEntry *entry)
{
    int pos = this->rowCount();
    this->beginInsertRows(QModelIndex(), pos, pos);
    this->entries << entry;
    this->sshConnByHost[entry->name] = entry;
    this->endInsertRows();
}

void SSHConnectionItemModel::removeConnectionEntry(SSHConnectionEntry *entry)
{

}

SSHConnectionEntry* SSHConnectionItemModel::getConnEntryByName(const QString name)
{
    return this->sshConnByHost[name];
}
