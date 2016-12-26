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
    this->shortDescription = json["shortDescription"].toString();
    this->hostname = json["hostname"].toString();
    this->username = json["username"].toString();
    this->password = this->decodePassword(json["password"].toString());
    this->sshkey = json["sshkey"].toString();
    this->port = json["port"].toInt();
    this->nextSessionNumber = json["nextSessionNumber"].toInt();
    this->notes = json["notes"].toString();

    for (QVariant cur: json["tabNames"].toArray().toVariantList()) {
        this->tabNames->append(cur.toString());
    }

    for (QVariant cur: json["hopHosts"].toArray().toVariantList()) {
        this->hopHosts.append(cur.toString());
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
    json["shortDescription"] = this->shortDescription;
    json["hostname"] = this->hostname;
    json["username"] = this->username;
    json["nextSessionNumber"] = (int) this->nextSessionNumber;
    json["notes"] = this->notes;
    json["password"] = this->encodePassword(this->password);
    json["sshkey"] = this->sshkey;
    json["port"] = (int) this->port;
    json["tabNames"] = QJsonArray::fromStringList(*this->tabNames);
    json["hopHosts"] = QJsonArray::fromStringList(this->hopHosts);
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

    // is this an indirect connection over one or more hops?
    if (!this->hopHosts.isEmpty()) {
        int i = 0;
        for (QString hopHost : this->hopHosts) {
            if (i > 0) {
                args.append("ssh");
            }
            args.append("-A");
            args.append("-t");
            args.append(QString("%1@%2").arg(this->username).arg(hopHost));

            i++;
        }

        args.append("ssh");
        args.append("-A");
        args.append("-t");
    }

    args.append(QString("%1@%2").arg(this->username).arg(this->hostname));

    return args;
}

QString SSHConnectionEntry::generateSSHCommand()
{
    QString cmd = "ssh ";

    cmd += this->generateCliArgs().join(" ");

    return cmd;
}

QString SSHConnectionEntry::generateSCPCommand(QString src, QString dest, bool dir)
{
    QString cmd = "scp";

    if (dir) {
        cmd += " -r";
    }

    QStringList cliArgs = this->generateCliArgs();
    for (int i = 0; i < cliArgs.size(); i++) {
        QString cur = cliArgs.at(i);

        // host is always the last argument
        if (i == cliArgs.size() - 1) {
            cmd += " " + src + " " + cur + ":" + dest;
            continue;
        }

        cmd += " " + cur;
    }

    return cmd;
}

const QString SSHConnectionEntry::encodePassword(const QString &password) const
{
    QByteArray base64 = password.toUtf8().toBase64();
    return QString::fromLatin1(base64);
}

const QString SSHConnectionEntry::decodePassword(const QString &cipherText) const
{
    QByteArray password = QByteArray::fromBase64(cipherText.toLatin1());
    return QString::fromUtf8(password);
}
