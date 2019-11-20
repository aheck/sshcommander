#include "SSHConnectionEntry.h"

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
    this->tabs = nullptr;
    this->tabNames = new QStringList();
    this->isAwsInstance = false;
    this->connection = nullptr;

    this->osType = OSType::Unknown;
    this->distroType = DistroType::Unknown;
    this->osMajorVersion = 0;
    this->osMinorVersion = 0;
    this->kernelMajorVersion = 0;
    this->kernelMinorVersion = 0;
}

SSHConnectionEntry::~SSHConnectionEntry()
{
    if (this->tabs) {
        delete this->tabs;
    }
}

void SSHConnectionEntry::read(const QJsonObject &json)
{
    this->name = json["name"].toString();
    this->shortDescription = json["shortDescription"].toString();
    this->hostname = json["hostname"].toString();
    this->username = json["username"].toString();
    this->password = Util::decryptString(json["password"].toString());
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

    for (QVariant cur: json["hopUsernames"].toArray().toVariantList()) {
        this->hopUsernames.append(cur.toString());
    }

    for (QVariant cur: json["hopSSHKeys"].toArray().toVariantList()) {
        this->hopSSHKeys.append(cur.toString());
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
    json["password"] = Util::encryptString(this->password);
    json["sshkey"] = this->sshkey;
    json["port"] = (int) this->port;
    json["tabNames"] = QJsonArray::fromStringList(*this->tabNames);
    json["hopHosts"] = QJsonArray::fromStringList(this->hopHosts);
    json["hopUsernames"] = QJsonArray::fromStringList(this->hopUsernames);
    json["hopSSHKeys"] = QJsonArray::fromStringList(this->hopSSHKeys);
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
        for (int i = 0; i < this->hopHosts.count(); i++) {
            QString hopHost = this->hopHosts.at(i);
            QString hopUsername;
            if (this->hopUsernames.count() >= i + 1) {
                hopUsername = this->hopUsernames.at(i);
            }

            QString hopSSHKey;
            if (this->hopSSHKeys.count() >= i + 1) {
                hopSSHKey = this->hopSSHKeys.at(i);
            }

            if (i > 0) {
                args.append("ssh");
            }

            args.append("-A");
            args.append("-t");

            if (!hopSSHKey.isEmpty()) {
                args.append("-i");
                args.append(hopSSHKey);
            }

            args.append(QString("%1@%2").arg(hopUsername).arg(hopHost));
        }

        args.append("ssh");
        args.append("-A");
        args.append("-t");
    }

    args.append(QString("%1@%2").arg(this->username).arg(this->hostname));

    return args;
}

QStringList SSHConnectionEntry::generateTunnelArgs(int localPort, int remotePort)
{
    QStringList args;

    //args.append("-o");
    //args.append("\"ExitOnForwardFailure yes\"");
    args.append("-L");
    args.append(QString::asprintf("%d:localhost:%d", localPort, remotePort));

    args.append(this->generateCliArgs());

    return args;
}

QStringList SSHConnectionEntry::generateSSHFSArgs(QString localDir, QString remoteDir)
{
    QStringList args;

    if (!this->sshkey.isEmpty()) {
        args.append("-o");
        args.append("IdentityFile=" + this->sshkey);
    }

    if (this->port != QString(DEFAULT_SSH_PORT).toInt(nullptr, 10)) {
        args.append("-p");
        args.append(QString(port));
    }

    args.append(this->username + "@" + this->hostname + ":" + remoteDir);
    args.append(localDir);
    args.append("-f");

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

QString SSHConnectionEntry::getIdentifier() const
{
    return this->username + "@" + this->hostname;
}
