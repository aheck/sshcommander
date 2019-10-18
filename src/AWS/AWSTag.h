/*****************************************************************************
 *
 * AWSTag represents a tag in AWS.
 *
 ****************************************************************************/

#ifndef AWSTAG_H
#define AWSTAG_H

#include <QString>
#include <QJsonObject>

class AWSTag
{
public:
    AWSTag();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString key;
    QString value;
};

#endif
