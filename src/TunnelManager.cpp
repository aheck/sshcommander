#include "TunnelManager.h"

TunnelEntry::TunnelEntry()
{
    this->termWidget = nullptr;
}

TunnelEntry::~TunnelEntry()
{
    if (this->termWidget != nullptr) {
        delete this->termWidget;
        this->termWidget = nullptr;
    }
}

void TunnelEntry::read(const QJsonObject &json)
{
    this->hostname = json["hostname"].toString();
    this->username = json["username"].toString();
    this->localPort = json["localPort"].toInt();
    this->remotePort = json["remotePort"].toInt();
    this->shortDescription = json["shortDescription"].toString();
    this->termWidget = nullptr;
}

void TunnelEntry::write(QJsonObject &json) const
{
    json["hostname"] = this->hostname;
    json["username"] = this->username;
    json["localPort"] = this->localPort;
    json["remotePort"] = this->remotePort;
    json["shortDescription"] = this->shortDescription;
}

bool TunnelEntry::isConnected()
{
    if (this->termWidget == nullptr) {
        return false;
    }

    int pid = this->termWidget->getShellPID();
    int inode = TunnelManager::findInodeListeningOnPort(this->localPort, "/proc/net/tcp");

    if (inode < 0) {
        return false;
    }

    bool isOwner = TunnelManager::isProcessOwnerOfSocketInode(pid, inode);
    return isOwner;
}

TunnelManager& TunnelManager::getInstance()
{
    static TunnelManager tunnelManager;
    return tunnelManager;
}

TunnelManager::TunnelManager()
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

    this->restoreFromJson();
}

TunnelManager::~TunnelManager()
{
}

bool TunnelManager::saveToJson()
{
    QJsonDocument jsonDoc;
    QJsonObject jsonObject;

    for (auto const& cur : this->tunnelsByConnection) {
        QString connectionName = cur.first;

        QJsonArray tunnelsArray = QJsonArray();

        for (std::shared_ptr<TunnelEntry> tunnel : this->tunnelsByConnection[connectionName]) {
            QJsonObject curObj;
            tunnel->write(curObj);
            tunnelsArray.append(curObj);
        }

        jsonObject[connectionName] = tunnelsArray;
    }

    jsonDoc.setObject(jsonObject);

    QDir jsonDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!jsonDir.exists()) {
        if (!jsonDir.mkpath(jsonDir.path())) {
            QMessageBox msgBox;
            msgBox.setText("Can't save SSH tunnels. Failed to create directory '" + jsonDir.path() + "'");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            return false;
        }
    }

    QString jsonFilePath = jsonDir.filePath("tunnels.json");
    QFile file(jsonFilePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Can't save SSH tunnels. Failed to open file '" + jsonFilePath + "' for writing.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return false;
    }

    qint64 bytesWritten = file.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        QMessageBox msgBox;
        msgBox.setText("Can't save SSH tunnels. Failed to write to file '" + jsonFilePath + "'");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    file.close();

    return true;
}

bool TunnelManager::restoreFromJson()
{
    QString filename = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("tunnels.json");
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Can't restore SSH tunnels. Failed to open " + filename);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return false;
    }

    QByteArray fileContent(file.readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);

    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray connArray(jsonObj["connections"].toArray());

    for (const QString& connectionName : jsonObj.keys()) {
        QJsonArray connArray(jsonObj[connectionName].toArray());

        for (QJsonValue curValue : connArray) {
            std::shared_ptr<TunnelEntry> tunnel = std::make_shared<TunnelEntry>();
            tunnel->read(curValue.toObject());

            this->tunnelsByConnection[connectionName].push_back(tunnel);
        }
    }

    file.close();

    return true;
}

int TunnelManager::countTunnels(QString username, QString hostname)
{
    QString connectionId = username + "@" + hostname;
    return this->tunnelsByConnection[connectionId].size();
}

std::shared_ptr<TunnelEntry> TunnelManager::getTunnel(QString username, QString hostname, int row)
{
    QString connectionId = username + "@" + hostname;
    auto tunnels = this->tunnelsByConnection[connectionId];

    if (row < 0 || row > tunnels.size() - 1) {
        return nullptr;
    }

    return tunnels[row];
}

std::shared_ptr<TunnelEntry> TunnelManager::getTunnel(QString username, QString hostname, int localPort, int remotePort)
{
    const QString connection = username + "@" + hostname;

    for (auto tunnel : this->tunnelsByConnection[connection]) {
        if (tunnel->localPort == localPort && tunnel->remotePort == remotePort) {
            return tunnel;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<TunnelEntry>> TunnelManager::getTunnels(QString username, QString hostname)
{
    QString connectionId = username + "@" + hostname;
    return this->tunnelsByConnection[connectionId];
}

void TunnelManager::createTunnel(std::shared_ptr<SSHConnectionEntry> connEntry, int localPort, int remotePort, QString shortDescription)
{
    const QString connection = connEntry->username + "@" + connEntry->hostname;
    std::weak_ptr<SSHConnectionEntry> connEntryWeak = connEntry;

    const QStringList args = connEntry->generateTunnelArgs(localPort, remotePort);
    SSHTermWidget *termWidget = new SSHTermWidget(&args, connEntryWeak, 0);
    termWidget->startShellProgram();

    std::shared_ptr<TunnelEntry> tunnelEntry = std::make_shared<TunnelEntry>();

    tunnelEntry->hostname = connEntry->hostname;
    tunnelEntry->username = connEntry->username;
    tunnelEntry->localPort = localPort;
    tunnelEntry->remotePort = remotePort;
    tunnelEntry->shortDescription = shortDescription;
    tunnelEntry->termWidget = termWidget;

    this->tunnelsByConnection[connection].push_back(tunnelEntry);

    std::cout << "SSH tunnel command: " << args.join(" ").toStdString() << "\n";
}

void TunnelManager::restartTunnel(std::shared_ptr<SSHConnectionEntry> connEntry, QString username, QString hostname, int localPort, int remotePort)
{
    auto tunnel = this->getTunnel(username, hostname, localPort, remotePort);
    std::weak_ptr<SSHConnectionEntry> connEntryWeak = connEntry;

    if (tunnel == nullptr) {
        return;
    }

    if (tunnel->termWidget != nullptr) {
        delete tunnel->termWidget;
        tunnel->termWidget = nullptr;
    }

    const QStringList args = connEntry->generateTunnelArgs(localPort, remotePort);
    tunnel->termWidget = new SSHTermWidget(&args, connEntryWeak, 0);
    tunnel->termWidget->startShellProgram();
}

bool TunnelManager::removeTunnel(QString username, QString hostname, int localPort, int remotePort)
{
    const QString connection = username + "@" + hostname;
    std::shared_ptr<TunnelEntry> tunnel = nullptr;

    int i = 0;
    for (auto cur : this->tunnelsByConnection[connection]) {
        if (cur->localPort == localPort && cur->remotePort == remotePort) {
            tunnel = cur;
            break;
        }

        i++;
    }

    if (tunnel == nullptr) {
        return false;
    }

    this->tunnelsByConnection[connection].erase(this->tunnelsByConnection[connection].begin() + i);
    delete tunnel->termWidget;
    tunnel->termWidget = nullptr;

    return true;
}

void TunnelManager::cleanUp()
{
    this->saveToJson();

    for (auto const& cur : this->tunnelsByConnection) {
        this->tunnelsByConnection[cur.first].clear();
    }
}

bool TunnelManager::isProcessOwnerOfSocketInode(int pid, int inode)
{
    QString path = QString("/proc/") + QString::number(pid) + "/fd/";

    QDir dir(path);
    if (!dir.exists()) {
        std::cerr << "Failed to open directory: " << path.toStdString() << "\n";
        return false;
    }

    for (QString filename : dir.entryList()) {
        if (filename == "." || filename == "..") {
            continue;
        }

        QString linkPath(path + filename);
        QFile linkFile(linkPath);
        QString linkText(linkFile.symLinkTarget());

        if (linkText.contains("socket:[")) {
            QStringList fields = linkText.split(":");
            QString socketNum = fields.at(1);
            socketNum.remove(0, 1);
            socketNum.chop(1);
            int socketInode = socketNum.toInt(nullptr, 10);

            if (socketInode == inode) {
                return true;
            }
        }
    }

    return false;
}

int TunnelManager::findInodeListeningOnPort(int port, QString procPath)
{
    QFile file(procPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }

    QRegExp sep("\\s+");

    // skip header line
    QByteArray lineBytes = file.readLine();

    while (!file.atEnd()) {
        lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes).trimmed();

        QStringList fields = line.split(sep);
        if (fields.length() < 2) {
            return false;
        }

        QString localAddress = fields.at(1);
        QStringList localAddressParts = localAddress.split(":");
        QString localPort = localAddressParts.at(1);

        QString state = fields.at(3);

        // The kernel lists the listening ports first. A listening address has
        // field st set to "0A". Once we see an entry with a different 
        // st value we don't need to continue reading the file.
        if (state != "0A") {
            break;
        }

        int localPortInt = localPort.toInt(nullptr, 16);

        if (port == localPortInt) {
            QString inode = fields.at(9);
            return inode.toInt(nullptr, 10);
        }
    }

    file.close();

    return -1;
}

bool TunnelManager::isLocalPortInUse(int port)
{
#ifdef Q_OS_LINUX
    if (TunnelManager::findInodeListeningOnPort(port, "/proc/net/tcp") > -1) {
        return true;
    }

    if (TunnelManager::findInodeListeningOnPort(port, "/proc/net/tcp6") > -1) {
        return true;
    }

    return false;
#elif Q_OS_MACOS
#endif
}
