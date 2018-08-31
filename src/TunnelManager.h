/*****************************************************************************
 *
 * TunnelManager is the singleton that manages all SSH tunnels created by SSH
 * Commander.
 *
 * It is used by all classes responsible for creating, listing and destroying
 * SSH tunnels.
 *
 ****************************************************************************/

#ifndef TUNNELMANAGER_H
#define TUNNELMANAGER_H

#include <map>
#include <memory>
#include <vector>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>

#include "SSHConnectionEntry.h"


struct TunnelEntry
{
    QString hostname;
    QString username;
    int localPort;
    int remotePort;
    QString shortDescription;
    SSHTermWidget *termWidget;

    TunnelEntry();
    ~TunnelEntry();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    bool isConnected();
};

class TunnelManager : public QObject
{
    Q_OBJECT

public:
    static TunnelManager& getInstance();

    TunnelManager(TunnelManager const &other) = delete;
    void operator=(TunnelManager const &) = delete;

    bool saveToJson();
    bool restoreFromJson();

    int countTunnels(QString username, QString hostname);
    std::shared_ptr<TunnelEntry> getTunnel(QString username, QString hostname, int row);
    std::shared_ptr<TunnelEntry> getTunnel(QString username, QString hostname, int localPort, int remotePort);
    std::vector<std::shared_ptr<TunnelEntry>> getTunnels(QString username, QString hostname);

    void createTunnel(std::shared_ptr<SSHConnectionEntry> connEntry, int localPort, int remoteport, QString shortDescription);
    void restartTunnel(std::shared_ptr<SSHConnectionEntry> connEntry, QString username, QString hostname, int localPort, int remotePort);
    bool removeTunnel(QString username, QString hostname, int localPort, int remotePort);

    static bool isProcessOwnerOfSocketInode(int pid, int inode);
    static int findInodeListeningOnPort(int port, QString procPath);
    static bool isLocalPortInUse(int port);

public slots:
    void cleanup();
    void tunnelTerminated(int exitStatus);

private:
    TunnelManager();
    ~TunnelManager();

    std::map<const QString, std::vector<std::shared_ptr<TunnelEntry>>> tunnelsByConnection;
};

#endif
