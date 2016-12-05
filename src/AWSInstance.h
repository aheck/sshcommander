#ifndef AWSINSTANCE_H
#define AWSINSTANCE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSSecurityGroup.h"
#include "AWSTag.h"

class AWSInstance
{
public:
    AWSInstance();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString name;
    QString region;
    QString availabilityZone;
    QString status;
    QString keyname;
    QString type;
    QString imageId;
    QString launchTime;
    QString publicIP;
    QString privateIP;
    QString subnetId;
    QString vpcId;
    QString virtualizationType;
    QString architecture;
    QString hypervisor;
    QString cfStackId;
    QString cfStackName;
    QList<AWSSecurityGroup> securityGroups;
    QList<AWSTag> tags;
};

#endif
