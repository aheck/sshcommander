#ifndef AWSINSTANCE_H
#define AWSINSTANCE_H

#include <QString>
#include <QJsonObject>

class AWSInstance
{
public:
    AWSInstance();
    ~AWSInstance();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString region;
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
};

#endif
