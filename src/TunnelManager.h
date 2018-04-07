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
};

class TunnelManager : public QObject
{
    Q_OBJECT

public:
    static TunnelManager& getInstance();

    TunnelManager(TunnelManager const &other) = delete;
    void operator=(TunnelManager const &) = delete;

    int countTunnels(QString username, QString hostname);
    std::shared_ptr<TunnelEntry> getTunnel(QString username, QString hostname, int row);
    std::vector<std::shared_ptr<TunnelEntry>> getTunnels(QString username, QString hostname);

    void createTunnel(std::shared_ptr<SSHConnectionEntry> connEntry, int localPort, int remoteport, QString shortDescription);
    void restartTunnel(QString username, QString hostname, int localPort, int remotePort);
    bool removeTunnel(QString username, QString hostname, int localPort, int remotePort);

public slots:
    void cleanUp();

private:
    TunnelManager();
    ~TunnelManager();

    std::map<const QString, std::vector<std::shared_ptr<TunnelEntry>>> tunnelsByConnection;
};

#endif
