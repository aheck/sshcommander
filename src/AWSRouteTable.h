#ifndef AWSROUTETABLE_H
#define AWSROUTETABLE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSRoute.h"

class AWSRouteTable
{
public:
    AWSRouteTable();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString vpcId;
    QString name;
    QList<AWSRoute> routes;
};

#endif
