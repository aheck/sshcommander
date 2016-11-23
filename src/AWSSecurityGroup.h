#ifndef AWSSECURITYGROUP_H
#define AWSSECURITYGROUP_H

#include <QString>
#include <QJsonObject>

class AWSSecurityGroup
{
public:
    AWSSecurityGroup();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString id;
    QString name;
};

#endif
