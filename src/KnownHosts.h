/*****************************************************************************
 *
 * KnownHosts provides an interface to read and write the known hosts file.
 *
 ****************************************************************************/

#ifndef KNOWNHOSTS_H
#define KNOWNHOSTS_H

#include <QDebug>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QMessageAuthenticationCode>
#include <QRegExp>
#include <QString>

enum class KnownHostsCheckResult
{
    NoMatch = 0,
    Match,
    Mismatch
};

class KnownHosts
{
public:
    static const QString getSSHClientConfigFilePath();
    static bool isHostnameHashingEnabled();
    static QString getKnownHostsFilePath();
    static bool isHostInKnownHostsFile(QString hostname);
    static bool addHostToKnownHostsFile(QString hostname, QString keyType, QString key);
    static bool removeHostFromKnownHostsFile(QString hostname);
    static bool replaceHostInKnownHostsFile(QString hostname, QString keyType, QString key);
    static KnownHostsCheckResult checkKey(QString hostname, QString key);

private:
    static bool isHostInKnownHostLine(QString hostname, QString line);
};

#endif
