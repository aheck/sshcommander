#ifndef AWSINSTANCE_H
#define AWSINSTANCE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSCache.h"
#include "AWSSecurityGroup.h"
#include "AWSTag.h"
#include "AWSVpc.h"

class AWSInstance
{
public:
    AWSInstance();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    const QString formattedName();
    const QString formattedImage();
    const QString formattedVpc();
    const QString formattedSubnet();

    void resolveReferences();
    void copyResolvedReferences(std::shared_ptr<AWSInstance> instance);

    QString id;
    QString name;
    QString region;
    QString availabilityZone;
    QString status;
    QString keyname;
    QString type;
    QString imageId;
    QString imageName;
    QString launchTime;
    QString publicIP;
    QString privateIP;
    QString subnetId;
    QString subnetName;
    QString vpcId;
    QString vpcName;
    QString virtualizationType;
    QString architecture;
    QString hypervisor;
    QString cfStackId;
    QString cfStackName;
    QString sourceDestCheck;
    QList<AWSSecurityGroup> securityGroups;
    QList<AWSTag> tags;
};

#endif
