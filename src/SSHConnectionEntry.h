#include <QStringList>

#include "AWSConnector.h"
#include "CustomTabWidget.h"

#ifndef SSHCONNECTIONENTRY_H
#define SSHCONNECTIONENTRY_H

struct SSHConnectionEntry
{
public:
    SSHConnectionEntry();
    ~SSHConnectionEntry();

    QString name;
    unsigned int nextSessionNumber;
    const QStringList *args;
    CustomTabWidget *tabs;
    AWSInstance *awsInstance;
};

#endif
