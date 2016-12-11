#include "SSHConnectionEntry.h"

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
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
    this->sshkey = json["sshkey"].toString();
    this->port = json["port"].toInt();
    this->nextSessionNumber = json["nextSessionNumber"].toInt();
    this->notes = json["notes"].toString();

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
    json["sshkey"] = this->sshkey;
    json["port"] = (int) this->port;
    json["tabNames"] = QJsonArray::fromStringList(*this->tabNames);
    json["isAwsInstance"] = this->isAwsInstance;
    if (this->isAwsInstance) {
        this->awsInstance->write(awsInstanceJson);
        json["awsInstance"] = awsInstanceJson;
    }
}

// build the argument list for ssh
QStringList SSHConnectionEntry::generateCliArgs()
{
    QStringList args;

    if (!this->sshkey.isEmpty()) {
        args.append("-i");
        args.append(sshkey);
    }

    if (this->port != QString(DEFAULT_SSH_PORT).toInt(nullptr, 10)) {
        args.append("-p");
        args.append(QString(port));
    }

    args.append(QString("%1@%2").arg(this->username).arg(this->hostname));

    return args;
}
