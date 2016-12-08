#include "SSHConnectionEntry.h"

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
    this->args = new QStringList();
    this->tabs = nullptr;
    this->tabNames = new QStringList();
    this->isAwsInstance = false;
}

SSHConnectionEntry::~SSHConnectionEntry()
{
    delete this->tabs;
}

void SSHConnectionEntry::read(const QJsonObject &json)
{
    this->name = json["name"].toString();
    this->hostname = json["hostname"].toString();
    this->username = json["username"].toString();
    this->nextSessionNumber = json["nextSessionNumber"].toInt();
    this->notes = json["notes"].toString();
    this->args = new QStringList();
    for (QVariant cur: json["args"].toArray().toVariantList()) {
        this->args->append(cur.toString());
    }

    for (QVariant cur: json["tabNames"].toArray().toVariantList()) {
        this->tabNames->append(cur.toString());
    }

    this->isAwsInstance = json["isAwsInstance"].toBool();
    if (this->isAwsInstance) {
        this->awsInstance = std::make_shared<AWSInstance>();
        this->awsInstance->read(json["awsInstance"].toObject());
    }
}

void SSHConnectionEntry::write(QJsonObject &json) const
{
    QJsonObject awsInstanceJson;

    json["name"] = this->name;
    json["hostname"] = this->hostname;
    json["username"] = this->username;
    json["nextSessionNumber"] = (int) this->nextSessionNumber;
    json["notes"] = this->notes;
    json["args"] = QJsonArray::fromStringList(*this->args);
    json["tabNames"] = QJsonArray::fromStringList(*this->tabNames);
    json["isAwsInstance"] = this->isAwsInstance;
    if (this->isAwsInstance) {
        this->awsInstance->write(awsInstanceJson);
        json["awsInstance"] = awsInstanceJson;
    }
}
