#include "AWSTag.h"

AWSTag::AWSTag()
{

}

void AWSTag::read(const QJsonObject &json)
{
    this->key = json["key"].toString();
    this->value = json["value"].toString();
}

void AWSTag::write(QJsonObject &json) const
{
    json["key"] = this->key;
    json["value"] = this->value;
}
