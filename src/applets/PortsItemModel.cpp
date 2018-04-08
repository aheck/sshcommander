#include "PortsItemModel.h"

std::map<QString, KnownPort> PortsItemModel::knownPorts {
    {":7", KnownPort{true, true, 7, "Echo", "Echo Protocol", "https://en.wikipedia.org/wiki/Echo_Protocol"}},
    {":13", KnownPort{true, true, 13, "Daytime", "Daytime Protocol", "https://en.wikipedia.org/wiki/Daytime_Protocol"}},
    {":20", KnownPort{true, true, 20, "FTP", "File Transfer Protocol (data transfer channel)", "https://en.wikipedia.org/wiki/File_Transfer_Protocol"}},
    {":21", KnownPort{true, true, 21, "FTP", "File Transfer Protocol (control channel)", "https://en.wikipedia.org/wiki/File_Transfer_Protocol"}},
    {":22", KnownPort{true, true, 22, "SSH", "Secure Shell Protocol", "https://en.wikipedia.org/wiki/Secure_Shell"}},
    {":23", KnownPort{true, true, 23, "Telnet", "Unencrypted remote shells", "https://en.wikipedia.org/wiki/Telnet"}},
    {":25", KnownPort{true, true, 25, "SMTP", "Simple Mail Transfer Protocol", "https://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol"}},
    {":53", KnownPort{true, true, 53, "DNS", "Domain Name System", "https://en.wikipedia.org/wiki/Domain_Name_System"}},
    {":69", KnownPort{true, true, 69, "TFTP", "Trivial File Transfer Protocol", "https://en.wikipedia.org/wiki/Trivial_File_Transfer_Protocol"}},
    {":70", KnownPort{true, true, 70, "Gopher", "Gopher Protocol", "https://en.wikipedia.org/wiki/Gopher_(protocol)"}},
    {":80", KnownPort{true, true, 80, "HTTP", "Hypertext Transfer Protocol (Webserver)", "https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol"}},
    {":109", KnownPort{true, true, 109, "POP 2", "Post Office Protocol Version 2", "https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol"}},
    {":110", KnownPort{true, true, 110, "POP 3", "Post Office Protocol Version 3", "https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol"}},
    {":443", KnownPort{true, true, 443, "HTTPS", "Hypertext Transfer Protocol over SSL/TLS (Webserver)", "https://en.wikipedia.org/wiki/HTTPS"}},
    {"udp:514", KnownPort{false, true, 514, "Syslog", "System logging daemon", "https://en.wikipedia.org/wiki/Syslog"}},
    {":3306", KnownPort{true, true, 3306, "MySQL", "SQL database system", "https://en.wikipedia.org/wiki/MySQL"}},
    {":5432", KnownPort{true, true, 5432, "PostgreSQL", "SQL database system", "https://en.wikipedia.org/wiki/PostgreSQL"}}
};

QString KnownPort::getCaption()
{
    return this->name + " - " + this->description;
}

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
        case static_cast<int>(PortColumns::Details):
            return QVariant(tr("Protocol Guess"));
    }

    return QVariant();
}

std::shared_ptr<NetstatEntry> PortsItemModel::getNetstatEntry(int row) const
{
    if (row < 0 || row > this->rowCount(QModelIndex()) - 1) {
        return nullptr;
    }

    return this->portsData.at(row);
}

QVariant PortsItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }

    std::shared_ptr<NetstatEntry> entry = this->getNetstatEntry(index.row());

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
        case (static_cast<int>(PortColumns::Details)):
            if (role != Qt::EditRole) {
                return QVariant();
            }

            bool success;
            KnownPort port = this->findKnownPort(&success, entry);

            if (success) {
                return QVariant("<a href=\"" + port.url + "\">" + port.getCaption() + "</a>");
            } else {
                return QVariant("?");
            }
    }

    return QVariant();
}

KnownPort PortsItemModel::findKnownPort(bool *success, std::shared_ptr<NetstatEntry> entry) const
{
    KnownPort port;

    *success = false;

    QString queryStringProto = entry->protocol + ":" + entry->localPort;
    QString queryStringAnyProto = ":" + entry->localPort;
    if (PortsItemModel::knownPorts.count(queryStringProto) != 0) {
        port = PortsItemModel::knownPorts[queryStringProto];
        *success = true;
    }

    if (PortsItemModel::knownPorts.count(queryStringAnyProto) != 0) {
        port = PortsItemModel::knownPorts[queryStringAnyProto];
        *success = true;
    }

    return port;
}

Qt::ItemFlags PortsItemModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
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

void PortsItemModel::sort(int column, Qt::SortOrder order)
{
    NetstatEntryComparator cmp;

    emit layoutAboutToBeChanged();

    cmp.column = column;

    if (order == Qt::DescendingOrder) {
        std::stable_sort(this->portsData.begin(), this->portsData.end(), cmp);
    } else {
        std::stable_sort(this->portsData.rbegin(), this->portsData.rend(), cmp);
    }

    emit layoutChanged();
}

bool NetstatEntryComparator::operator() (const std::shared_ptr<NetstatEntry> &a, const std::shared_ptr<NetstatEntry> &b) {
    bool result = false;

    if (a.get() == b.get()) {
        return false;
    }

    bool ok;
    int aint, bint;

    switch (this->column) {
        case static_cast<int>(PortColumns::Protocol):
            result = a->protocol.compare(b->protocol) < 0;
            break;
        case static_cast<int>(PortColumns::LocalAddress):
            result = a->localAddress.compare(b->localAddress) < 0;
            break;
        case static_cast<int>(PortColumns::LocalPort):
            aint = a->localPort.toInt(&ok);
            bint = b->localPort.toInt(&ok);
            result = aint < bint;
            break;
        case static_cast<int>(PortColumns::ForeignAddress):
            result = a->foreignAddress.compare(b->foreignAddress) < 0;
            break;
        case static_cast<int>(PortColumns::ForeignPort):
            result = a->foreignPort.compare(b->foreignPort) < 0;
            break;
        case static_cast<int>(PortColumns::State):
            result = a->state.compare(b->state) < 0;
            break;
    }

    return result;
}
