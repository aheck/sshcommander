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
    this->imageName = json["imageName"].toString();
    this->launchTime = json["launchTime"].toString();
    this->publicIP = json["publicIP"].toString();
    this->privateIP = json["privateIP"].toString();
    this->subnetId = json["subnetId"].toString();
    this->subnetName = json["subnetName"].toString();
    this->vpcId = json["vpcId"].toString();
    this->vpcName = json["vpcName"].toString();
    this->virtualizationType = json["virtualizationType"].toString();
    this->architecture = json["architecture"].toString();
    this->hypervisor = json["hypervisor"].toString();
    this->cfStackId = json["cfStackId"].toString();
    this->cfStackName = json["cfStackName"].toString();
    this->sourceDestCheck = json["sourceDestCheck"].toString();

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
    json["imageName"] = this->imageName;
    json["launchTime"] = this->launchTime;
    json["publicIP"] = this->publicIP;
    json["privateIP"] = this->privateIP;
    json["subnetId"] = this->subnetId;
    json["subnetName"] = this->subnetName;
    json["vpcId"] = this->vpcId;
    json["vpcName"] = this->vpcName;
    json["virtualizationType"] = this->virtualizationType;
    json["architecture"] = this->architecture;
    json["hypervisor"] = this->hypervisor;
    json["cfStackId"] = this->cfStackId;
    json["cfStackName"] = this->cfStackName;
    json["sourceDestCheck"] = this->sourceDestCheck;

    QJsonArray securityGroupsArray;
    for (AWSSecurityGroup securityGroup: this->securityGroups) {
        QJsonObject obj;
        securityGroup.write(obj);
        securityGroupsArray.append(obj);
    }
    json["securityGroups"] = securityGroupsArray;

    QJsonArray tagsArray;
    for (AWSTag tag: this->tags) {
        QJsonObject obj;
        tag.write(obj);
        tagsArray.append(obj);
    }
    json["tags"] = tagsArray;
}

const QString AWSInstance::formattedName()
{
    if (!this->name.isEmpty()) {
        return this->name + " (" + this->id + ")";
    }

    return this->id;
}

const QString AWSInstance::formattedImage()
{
    if (!this->imageName.isEmpty()) {
        return this->imageName + " (" + this->imageId + ")";
    }

    return this->imageId;
}

const QString AWSInstance::formattedVpc()
{
    if (!this->vpcName.isEmpty()) {
        return this->vpcName + " (" + this->vpcId + ")";
    }

    return this->vpcId;
}

const QString AWSInstance::formattedSubnet()
{
    if (!this->subnetName.isEmpty()) {
        return this->subnetName + " (" + this->subnetId + ")";
    }

    return this->subnetId;
}

void AWSInstance::resolveReferences()
{
    AWSCache& cache = AWSCache::getInstance();

    std::shared_ptr<AWSVpc> vpc = cache.resolveVpc(this->region, this->vpcId);
    if (vpc != nullptr) {
        this->vpcName = vpc->name;
    }

    std::shared_ptr<AWSSubnet> subnet = cache.resolveSubnet(this->region, this->subnetId);
    if (subnet != nullptr) {
        this->subnetName = subnet->name;
    }
}

void AWSInstance::copyResolvedReferences(std::shared_ptr<AWSInstance> other)
{
    if (other == nullptr) {
        return;
    }

    if (this->imageId.isEmpty()) {
        this->imageId = other->imageId;
    }

    if (this->imageName.isEmpty()) {
        this->imageName = other->imageName;
    }

    if (this->subnetId.isEmpty()) {
        this->subnetId = other->subnetId;
    }

    if (this->subnetName.isEmpty()) {
        this->subnetName = other->subnetName;
    }

    if (this->vpcId.isEmpty()) {
        this->vpcId = other->vpcId;
    }

    if (this->vpcName.isEmpty()) {
        this->vpcName = other->vpcName;
    }
}
