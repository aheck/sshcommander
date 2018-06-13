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
    return 0;
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
    }

    return QVariant();
}

QVariant FileTransfersItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags FileTransfersItemModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}
