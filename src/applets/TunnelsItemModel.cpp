#include "TunnelsItemModel.h"

QModelIndex TunnelsItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex TunnelsItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int TunnelsItemModel::rowCount(const QModelIndex &parent) const
{
    if (this->username == "" || this->hostname == "") {
        return 0;
    }

    return TunnelManager::getInstance().countTunnels(this->username, this->hostname);
}

int TunnelsItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(TunnelColumns::Count);
}

QVariant TunnelsItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(TunnelColumns::LocalPort):
            return QVariant(tr("LocalPort"));
        case static_cast<int>(TunnelColumns::RemotePort):
            return QVariant(tr("RemotePort"));
        case static_cast<int>(TunnelColumns::ShortDescription):
                return QVariant(tr("Short Description"));
        case static_cast<int>(TunnelColumns::Connected):
            return QVariant(tr("Connected"));
    }

    return QVariant();
}

QVariant TunnelsItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    std::shared_ptr<TunnelEntry> tunnel = TunnelManager::getInstance().getTunnel(this->username, this->hostname, index.row());

    if (tunnel == nullptr) {
        return QVariant();
    }

    switch (index.column()) {
        case (static_cast<int>(TunnelColumns::LocalPort)):
            return QVariant(tunnel->localPort);
        case (static_cast<int>(TunnelColumns::RemotePort)):
            return QVariant(tunnel->remotePort);
        case (static_cast<int>(TunnelColumns::ShortDescription)):
            return QVariant(tunnel->shortDescription);
        case (static_cast<int>(TunnelColumns::Connected)):
            if (role== Qt::DecorationRole) {
                if (tunnel->isConnected()) {
                    return QIcon(":/images/green-light.svg");
                } else {
                    return QIcon(":/images/red-light.svg");
                }
            }

            return QVariant("");
    }

    return QVariant();
}

void TunnelsItemModel::clear()
{
    emit layoutAboutToBeChanged();

    emit layoutChanged();
}

void TunnelsItemModel::updateData(QString data)
{
}

void TunnelsItemModel::sort(int column, Qt::SortOrder order)
{
}

void TunnelsItemModel::setConnectionStrings(QString username, QString hostname)
{
    beginResetModel();

    this->username = username;
    this->hostname = hostname;

    endResetModel();
}

void TunnelsItemModel::reloadData()
{
    this->beginResetModel();
    this->endResetModel();
}
