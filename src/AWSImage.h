/*****************************************************************************
 *
 * AWSImage represents an disk image entry in AWS.
 *
 ****************************************************************************/

#ifndef AWSIMAGE_H
#define AWSIMAGE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>

#include "AWSTag.h"

class AWSImage
{
public:
    AWSImage();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString location;
    QString state;
    QString ownerId;
    bool isPublic;
    QString architecture;
    QString type;
    QString kernelId;
    QString ramdiskId;
    QString ownerAlias;
    QString name;
    QString description;
    QString rootDeviceType;
    QString rootDeviceName;
    QString virtualizationType;
    QString hypervisor;
    QString creationDate;
    QList<AWSTag> tags;
};

#endif
