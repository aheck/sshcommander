#include "KnownHosts.h"

const QString KnownHosts::getSSHClientConfigFilePath()
{
    return "/etc/ssh/ssh_config";
}

bool KnownHosts::isHostnameHashingEnabled()
{
    QString filePath = KnownHosts::getSSHClientConfigFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file: " << filePath;
        return false;
    }

    QRegExp reg("^\\s*HashKnownHosts\\s+yes\\s*$");

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (reg.exactMatch(line)) {
            return true;
        }
    }

    return false;
}

QString KnownHosts::getKnownHostsFilePath()
{
    QString path;

    path = QDir::homePath() + "/.ssh/known_hosts";

    return path;
}

bool KnownHosts::isHostInKnownHostLine(QString hostname, QString line)
{
    QStringList fields = line.split(" ");
    if (fields.length() < 2) {
        return false;
    }

    QString hostField = fields.at(0);

    // SHA1 HMAC hashed hostnames?
    if (hostField.startsWith("|1|")) {
        hostField.remove(0, 3); // remove hash marker |1|
        QStringList hashFields = hostField.split("|");
        if (hashFields.length() != 2) {
            return false;
        }

        QByteArray key = QByteArray::fromBase64(hashFields.at(0).toLatin1());
        QByteArray hashedHostname = QByteArray::fromBase64(hashFields.at(1).toLatin1());

        QByteArray hostnameHashed = QMessageAuthenticationCode::hash(hostname.toLatin1(), key, QCryptographicHash::Sha1);

        if (hashedHostname == hostnameHashed) {
            return true;
        }
    } else { // plain text hostnames
        QStringList hostnames = hostField.split(",");

        for (QString curHostname : hostnames) {
            if (curHostname == hostname) {
                return true;
            }
        }
    }

    return false;
}

bool KnownHosts::isHostInKnownHostsFile(QString hostname)
{
    QString filePath = KnownHosts::getKnownHostsFilePath();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (isHostInKnownHostLine(hostname, line)) {
            return true;
        }
    }

    return false;
}

bool KnownHosts::addHostToKnownHostsFile(QString hostname, QString keyType, QString key)
{
    QString line;

    if (KnownHosts::isHostnameHashingEnabled()) {
        QString hashedHostname = "|1|";

        //hashKey = ;

        line = hashedHostname + " " + keyType + " " + key;
    } else {
        line = hostname + " " + keyType + " " + key;
    }

    QFile file(QDir::home().filePath(".ssh/known_hosts"));
    if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            return false;
        }
    } else {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            return false;
        }
    }

    QTextStream stream(&file);
    stream << ("\n");
    stream << (line);

    file.close();

    return true;
}

bool KnownHosts::removeHostFromKnownHostsFile(QString hostname)
{
    QStringList linesToWrite;
    QString filePath = KnownHosts::getKnownHostsFilePath();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (!isHostInKnownHostLine(hostname, line)) {
            linesToWrite.append(line);
        }
    }

    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    for (QString line : linesToWrite) {
        file.write(line.toLatin1());
    }

    return true;
}

bool KnownHosts::replaceHostInKnownHostsFile(QString hostname, QString keyType, QString key)
{
    if (KnownHosts::removeHostFromKnownHostsFile(hostname)) {
        return KnownHosts::addHostToKnownHostsFile(hostname, keyType, key);
    }

    return false;
}

KnownHostsCheckResult KnownHosts::checkKey(QString hostname, QString key)
{
    QString filePath = KnownHosts::getKnownHostsFilePath();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return KnownHostsCheckResult::NoMatch;
    }

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (isHostInKnownHostLine(hostname, line)) {
            if (line.endsWith(key)) {
                return KnownHostsCheckResult::Match;
            } else {
                return KnownHostsCheckResult::Mismatch;
            }
        }
    }

    return KnownHostsCheckResult::NoMatch;
}
