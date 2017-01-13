#ifndef AWSVPC_H
#define AWSVPC_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSTag.h"

class AWSVpc
{
public:
    AWSVpc();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString name;
    QString state;
    QString cidrBlock;
    QString dhcpOptionsId;
    QString instanceTenancy;
    bool isDefault;
    QList<AWSTag> tags;
};

#endif
