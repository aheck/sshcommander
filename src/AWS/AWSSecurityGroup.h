/*****************************************************************************
 *
 * AWSSecurityGroup represents a security group in AWS.
 *
 ****************************************************************************/

#ifndef AWSSECURITYGROUP_H
#define AWSSECURITYGROUP_H

#include <QJsonObject>
#include <QList>
#include <QString>

class AWSIngressPermission {
public:
    QString ipProtocol;
    QString fromPort;
    QString toPort;

    QList<QString> cidrs;
};

class AWSEgressPermission {
public:
    QString ipProtocol;
    QString fromPort;
    QString toPort;

    QList<QString> cidrs;
};

class AWSSecurityGroup
{
public:
    AWSSecurityGroup();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString name;
    QString description;
    QString vpcId;

    QList<AWSIngressPermission> ingressPermissions;
    QList<AWSEgressPermission> egressPermissions;
};

#endif
