#ifndef SSHCONNECTIONENTRY_H
#define SSHCONNECTIONENTRY_H

#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

#include "AWSConnector.h"
#include "CustomTabWidget.h"

struct SSHConnectionEntry
{
public:
    SSHConnectionEntry();
    ~SSHConnectionEntry();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString name;
    QString hostname;
    QString username;
    unsigned int nextSessionNumber;
    QStringList *args;
    CustomTabWidget *tabs;
    bool isAwsInstance;
    AWSInstance awsInstance;
};

#endif
