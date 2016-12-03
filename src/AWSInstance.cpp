#include "AWSInstance.h"

AWSInstance::AWSInstance()
{

}

void AWSInstance::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->name = json["name"].toString();
    this->region = json["region"].toString();
    this->availabilityZone = json["availabilityZone"].toString();
    this->status = json["status"].toString();
    this->keyname = json["keyname"].toString();
    this->type = json["type"].toString();
    this->imageId = json["imageId"].toString();
    this->launchTime = json["launchTime"].toString();
    this->publicIP = json["publicIP"].toString();
    this->privateIP = json["privateIP"].toString();
    this->subnetId = json["subnetId"].toString();
    this->vpcId = json["vpcId"].toString();
    this->virtualizationType = json["virtualizationType"].toString();
    this->architecture = json["architecture"].toString();
    this->hypervisor = json["hypervisor"].toString();

    for (QJsonValueRef ref: json["securityGroups"].toArray()) {
        AWSSecurityGroup securityGroup;
        securityGroup.read(ref.toObject());
        this->securityGroups.append(securityGroup);
    }

    for (QJsonValueRef ref: json["tags"].toArray()) {
        AWSTag tag;
        tag.read(ref.toObject());
        this->tags.append(tag);
    }
}

void AWSInstance::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["name"] = this->name;
    json["region"] = this->region;
    json["availabilityZone"] = this->availabilityZone;
    json["status"] = this->status;
    json["keyname"] = this->keyname;
    json["type"] = this->type;
    json["imageId"] = this->imageId;
    json["launchTime"] = this->launchTime;
    json["publicIP"] = this->publicIP;
    json["privateIP"] = this->privateIP;
    json["subnetId"] = this->subnetId;
    json["vpcId"] = this->vpcId;
    json["virtualizationType"] = this->virtualizationType;
    json["architecture"] = this->architecture;
    json["hypervisor"] = this->hypervisor;

    QJsonArray securityGroups;
    for (AWSSecurityGroup securityGroup: this->securityGroups) {
        QJsonObject obj;
        securityGroup.write(obj);
        securityGroups.append(obj);
    }
    json["securityGroups"] = securityGroups;

    QJsonArray tags;
    for (AWSTag tag: this->tags) {
        QJsonObject obj;
        tag.write(obj);
        tags.append(obj);
    }
    json["tags"] = tags;
}