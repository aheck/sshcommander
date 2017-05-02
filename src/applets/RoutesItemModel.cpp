#include "RoutesItemModel.h"

QModelIndex RoutesItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex RoutesItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int RoutesItemModel::rowCount(const QModelIndex &parent) const
{
    return this->routesData.size();
}

int RoutesItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(RouteColumns::Count);
}

QVariant RoutesItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(RouteColumns::Destination):
            return QVariant(tr("Destination"));
        case static_cast<int>(RouteColumns::Gateway):
            return QVariant(tr("Gateway"));
        case static_cast<int>(RouteColumns::Genmask):
            return QVariant(tr("Genmask"));
        case static_cast<int>(RouteColumns::Interface):
            return QVariant(tr("Interface"));
    }

    return QVariant();
}

QVariant RoutesItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    std::shared_ptr<RouteEntry> entry = this->routesData.at(index.row());

    switch (index.column()) {
        case (static_cast<int>(RouteColumns::Destination)):
            return QVariant(entry->destination);
        case (static_cast<int>(RouteColumns::Gateway)):
            return QVariant(entry->gateway);
        case (static_cast<int>(RouteColumns::Genmask)):
            return QVariant(entry->genmask);
        case (static_cast<int>(RouteColumns::Interface)):
            return QVariant(entry->interface);
    }

    return QVariant();
}

void RoutesItemModel::clear()
{
    emit layoutAboutToBeChanged();

    this->routesData.clear();

    emit layoutChanged();
}

void RoutesItemModel::updateData(QString data)
{
    this->clear();

    emit layoutAboutToBeChanged();

    QStringList lines = data.trimmed().split("\n");

    // remove the two header lines
    if (lines.count() > 2) {
        lines.removeFirst();
        lines.removeFirst();
    }

    QRegularExpression spaceRegExp("\\s+");

    std::vector<std::shared_ptr<RouteEntry>> entries;

    for (int i = 0; i < lines.count(); i++) {
        QString line = lines.at(i);
        QStringList fields = line.split(spaceRegExp);

        if (fields.count() < 8) {
            continue;
        }

        std::shared_ptr<RouteEntry> entry = std::make_shared<RouteEntry>();
        entries.push_back(entry);

        entry->destination = fields.at(0);
        entry->gateway = fields.at(1);
        entry->genmask = fields.at(2);
        entry->interface = fields.at(7);
    }

    this->routesData = entries;

    emit layoutChanged();
}
