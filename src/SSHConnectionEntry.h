#ifndef SSHCONNECTIONENTRY_H
#define SSHCONNECTIONENTRY_H

#include <memory>

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "CustomTabWidget.h"

#include "globals.h"

struct SSHConnectionEntry
{
public:
    SSHConnectionEntry();
    ~SSHConnectionEntry();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    QStringList generateCliArgs();
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
    CustomTabWidget *tabs;
    QStringList *tabNames;
    QStringList hopHosts;
    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;
};

#endif
