#include "AWSSecurityGroup.h"

AWSSecurityGroup::AWSSecurityGroup()
{

}

void AWSSecurityGroup::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->name = json["name"].toString();
}

void AWSSecurityGroup::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["name"] = this->name;
}
