#include "PortsItemModel.h"

QModelIndex PortsItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex PortsItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int PortsItemModel::rowCount(const QModelIndex &parent) const
{
    return this->portsData.size();
}

int PortsItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(PortColumns::Count);
}

QVariant PortsItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(PortColumns::Protocol):
            return QVariant(tr("Protocol"));
        case static_cast<int>(PortColumns::LocalAddress):
            return QVariant(tr("Local Address"));
        case static_cast<int>(PortColumns::LocalPort):
            return QVariant(tr("Local Port"));
        case static_cast<int>(PortColumns::ForeignAddress):
            return QVariant(tr("Foreign Address"));
        case static_cast<int>(PortColumns::ForeignPort):
            return QVariant(tr("Foreign Port"));
        case static_cast<int>(PortColumns::State):
            return QVariant(tr("State"));
    }

    return QVariant();
}

QVariant PortsItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    std::shared_ptr<NetstatEntry> entry = this->portsData.at(index.row());

    switch (index.column()) {
        case (static_cast<int>(PortColumns::Protocol)):
            return QVariant(entry->protocol);
        case (static_cast<int>(PortColumns::LocalAddress)):
            return QVariant(entry->localAddress);
        case (static_cast<int>(PortColumns::LocalPort)):
            return QVariant(entry->localPort);
        case (static_cast<int>(PortColumns::ForeignAddress)):
            return QVariant(entry->foreignAddress);
        case (static_cast<int>(PortColumns::ForeignPort)):
            return QVariant(entry->foreignPort);
        case (static_cast<int>(PortColumns::State)):
            return QVariant(entry->state);
    }

    return QVariant();
}

void PortsItemModel::clear()
{
    emit layoutAboutToBeChanged();

    this->portsData.clear();

    emit layoutChanged();
}

void PortsItemModel::updateData(QString data)
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
    QRegularExpression hostnameRegExp("^(.*):(\\d+|\\*)$");

    std::vector<std::shared_ptr<NetstatEntry>> entries;

    for (int i = 0; i < lines.count(); i++) {
        QString line = lines.at(i);
        QStringList fields = line.split(spaceRegExp);

        if (fields.count() < 5) {
            continue;
        }

        std::shared_ptr<NetstatEntry> entry = std::make_shared<NetstatEntry>();
        entries.push_back(entry);

        entry->protocol = fields.at(0);

        // local address and local port
        QString localAddressAndPort = fields.at(3);

        QRegularExpressionMatch match = hostnameRegExp.match(localAddressAndPort);
        if (match.hasMatch()) {
            entry->localAddress = match.captured(1);
            entry->localPort = match.captured(2);
        }

        // foreign address and foreign port
        QString foreignAddressAndPort = fields.at(4);

        match = hostnameRegExp.match(foreignAddressAndPort);
        if (match.hasMatch()) {
            entry->foreignAddress = match.captured(1);
            entry->foreignPort = match.captured(2);
        }

        if (fields.count() >= 6) {
            entry->state = fields.at(5);
        }
    }

    this->portsData = entries;

    emit layoutChanged();
}
