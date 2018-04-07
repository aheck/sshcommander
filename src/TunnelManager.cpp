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

TunnelManager& TunnelManager::getInstance()
{
    static TunnelManager tunnelManager;
    return tunnelManager;
}

TunnelManager::TunnelManager()
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));
}

TunnelManager::~TunnelManager()
{
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

void TunnelManager::restartTunnel(QString username, QString hostname, int localport, int remoteport)
{

}

bool TunnelManager::removeTunnel(QString username, QString hostname, int localPort, int remotePort)
{
    const QString connection = username + "@" + hostname;
    std::shared_ptr<TunnelEntry> tunnel = nullptr;

    int i = 0;
    for (auto cur : this->tunnelsByConnection[connection]) {
        i++;
        if (cur->localPort == localPort && cur->remotePort == remotePort) {
            tunnel = cur;
            break;
        }
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
    for (auto const& cur : this->tunnelsByConnection) {
        this->tunnelsByConnection[cur.first].clear();
    }
}
