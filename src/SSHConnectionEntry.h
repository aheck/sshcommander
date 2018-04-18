/*****************************************************************************
 *
 * SSHConnectionEntry represents a SSH connection and all its settings.
 *
 * In case of connections to AWS instances it also holds a shared pointer
 * to the corresponding AWSInstance object.
 *
 ****************************************************************************/

#ifndef SSHCONNECTIONENTRY_H
#define SSHCONNECTIONENTRY_H

#include <memory>
#include <mutex>

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "SSHConnection.h"
#include "TabbedTerminalWidget.h"

#include "globals.h"

class TabbedTerminalWidget;

struct SSHConnectionEntry
{
public:
    SSHConnectionEntry();
    ~SSHConnectionEntry();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    QStringList generateCliArgs();
    QStringList generateTunnelArgs(int localPort, int remotePort);
    QStringList generateSSHFSArgs(QString localDir, QString remoteDir);
    QString generateSSHCommand();
    QString generateSCPCommand(QString src, QString dest, bool dir = false);
    const QString encodePassword(const QString &password) const;
    const QString decodePassword(const QString &cipherText) const;

    QString name;
    QString shortDescription;
    QString hostname;
    QString username;
    QString password;

    QString sshkey;
    int port;
    unsigned int nextSessionNumber;
    QString notes;
    TabbedTerminalWidget *tabs;
    QStringList *tabNames;
    QStringList hopHosts;
    QStringList hopUsernames;
    QStringList hopSSHKeys;
    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;

    // A libssh2 connection to the instance. All access is managed by
    // SSHConnectionManager.
    std::mutex connectionMutex;
    std::shared_ptr<SSHConnection> connection;
};

#endif
