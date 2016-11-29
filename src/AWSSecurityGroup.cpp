#include "AWSSecurityGroup.h"

AWSSecurityGroup::AWSSecurityGroup()
{

}

void AWSSecurityGroup::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->name = json["name"].toString();
    this->description = json["description"].toString();
    this->vpcId = json["vpcId"].toString();
}

void AWSSecurityGroup::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["name"] = this->name;
    json["description"] = this->description;
    json["vpcId"] = this->vpcId;
}
