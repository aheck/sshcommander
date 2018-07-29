/*****************************************************************************
 *
 * AWSRoute represents an route entry in AWS.
 *
 ****************************************************************************/

#ifndef AWSROUTE_H
#define AWSROUTE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

class AWSRoute
{
public:
    AWSRoute();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString destinationCidrBlock;
    QString destinationIpv6CidrBlock;
    QString gatewayId;
    QString state;
    QString origin;
};

#endif
