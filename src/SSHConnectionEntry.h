#ifndef SSHCONNECTIONENTRY_H
#define SSHCONNECTIONENTRY_H

#include <memory>

#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

#include "AWSConnector.h"
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

    QString name;
    QString hostname;
    QString username;
    QString sshkey;
    int port;
    unsigned int nextSessionNumber;
    QString notes;
    CustomTabWidget *tabs;
    QStringList *tabNames;
    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;
};

#endif
