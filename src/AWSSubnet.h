/*****************************************************************************
 *
 * AWSSubnet represents an subnet entry in AWS.
 *
 ****************************************************************************/

#ifndef AWSSUBNET_H
#define AWSSUBNET_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSTag.h"

class AWSSubnet
{
public:
    AWSSubnet();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString name;
    QString state;
    QString vpcId;
    QString cidrBlock;
    QString availableIpAddressCount;
    QString availabilityZone;
    bool defaultForAz;
    bool mapPublicIpOnLaunch;
    QList<AWSTag> tags;
};

#endif
