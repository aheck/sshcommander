#include "AWSImage.h"

AWSImage::AWSImage()
{

}

void AWSImage::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->location = json["location"].toString();
    this->state = json["state"].toString();
    this->ownerId = json["ownerId"].toString();
    this->isPublic = json["isPublic"].toBool();
    this->architecture = json["architecture"].toString();
    this->type = json["type"].toString();
    this->kernelId = json["kernelId"].toString();
    this->ramdiskId = json["ramdiskId"].toString();
    this->ownerAlias = json["ownerAlias"].toString();
    this->name = json["name"].toString();
    this->description = json["description"].toString();
    this->rootDeviceType = json["rootDeviceType"].toString();
    this->rootDeviceName = json["rootDeviceName"].toString();
    this->virtualizationType = json["virtualizationType"].toString();
    this->hypervisor = json["hypervisor"].toString();
    this->creationDate = json["creationDate"].toString();

    for (QJsonValueRef ref: json["tags"].toArray()) {
        AWSTag tag;
        tag.read(ref.toObject());
        this->tags.append(tag);
    }
}

void AWSImage::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["location"] = this->location;
    json["state"] = this->state;
    json["ownerId"] = this->ownerId;
    json["isPublic"] = this->isPublic;
    json["architecture"] = this->architecture;
    json["type"] = this->type;
    json["kernelId"] = this->kernelId;
    json["ramdiskId"] = this->ramdiskId;
    json["ownerAlias"] = this->ownerAlias;
    json["name"] = this->name;
    json["description"] = this->description;
    json["rootDeviceType"] = this->rootDeviceType;
    json["rootDeviceName"] = this->rootDeviceName;
    json["virtualizationType"] = this->virtualizationType;
    json["hypervisor"] = this->hypervisor;
    json["creationDate"] = this->creationDate;

    QJsonArray tags;
    for (AWSTag tag: this->tags) {
        QJsonObject obj;
        tag.write(obj);
        tags.append(obj);
    }
    json["tags"] = tags;
}
