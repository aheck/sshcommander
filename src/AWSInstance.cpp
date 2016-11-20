#include "AWSInstance.h"

AWSInstance::AWSInstance()
{

}

AWSInstance::~AWSInstance()
{

}

void AWSInstance::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->region = json["region"].toString();
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
}

void AWSInstance::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["region"] = this->region;
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
}
