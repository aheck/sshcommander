#include "AWSVpc.h"

AWSVpc::AWSVpc()
{

}

void AWSVpc::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->name = json["name"].toString();
    this->state = json["state"].toString();
    this->cidrBlock = json["cidrBlock"].toString();
    this->dhcpOptionsId = json["dhcpOptionsId"].toString();
    this->instanceTenancy = json["instanceTenancy"].toString();
    this->isDefault = json["isDefault"].toBool();

    for (QJsonValueRef ref: json["tags"].toArray()) {
        AWSTag tag;
        tag.read(ref.toObject());
        this->tags.append(tag);
    }
}

void AWSVpc::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["name"] = this->name;
    json["state"] = this->state;
    json["cidrBlock"] = this->cidrBlock;
    json["dhcpOptionsId"] = this->dhcpOptionsId;
    json["instanceTenancy"] = this->instanceTenancy;
    json["isDefault"] = this->isDefault;

    QJsonArray tags;
    for (AWSTag tag: this->tags) {
        QJsonObject obj;
        tag.write(obj);
        tags.append(obj);
    }
    json["tags"] = tags;
}
