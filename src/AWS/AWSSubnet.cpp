#include "AWSSubnet.h"

AWSSubnet::AWSSubnet()
{

}

void AWSSubnet::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->name = json["name"].toString();
    this->state = json["state"].toString();
    this->vpcId = json["vpcId"].toString();
    this->cidrBlock = json["cidrBlock"].toString();
    this->availableIpAddressCount = json["availableIpAddressCount"].toString();
    this->availabilityZone = json["availabilityZone"].toString();
    this->defaultForAz = json["defaultForAz"].toBool();
    this->mapPublicIpOnLaunch = json["mapPublicIpOnLaunch"].toBool();

    for (QJsonValueRef ref: json["tags"].toArray()) {
        AWSTag tag;
        tag.read(ref.toObject());
        this->tags.append(tag);
    }
}

void AWSSubnet::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["name"] = this->name;
    json["state"] = this->state;
    json["vpcId"] = this->vpcId;
    json["cidrBlock"] = this->cidrBlock;
    json["availableIpAddressCount"] = this->availableIpAddressCount;
    json["availabilityZone"] = this->availabilityZone;
    json["defaultForAz"] = this->defaultForAz;
    json["mapPublicIpOnLaunch"] = this->mapPublicIpOnLaunch;

    QJsonArray tags;
    for (AWSTag tag: this->tags) {
        QJsonObject obj;
        tag.write(obj);
        tags.append(obj);
    }
    json["tags"] = tags;
}
