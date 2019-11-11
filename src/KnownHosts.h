/*****************************************************************************
 *
 * KnownHosts provides an interface to read and write the known hosts file.
 *
 ****************************************************************************/

#ifndef KNOWNHOSTS_H
#define KNOWNHOSTS_H

#include <iostream>

#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageAuthenticationCode>
#include <QRegExp>
#include <QString>

#include "Util.h"

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
    static bool isHostnameHashingEnabled(QString clientConfigPath = "");
    static QString getKnownHostsFilePath();
    static bool isHostInKnownHostsFile(QString hostname, QString knownHostsFilePath="");
    static bool addHostToKnownHostsFile(QString hostname, QString keyType, QString key, QString knownHostsFilePath="");
    static bool removeHostFromKnownHostsFile(QString hostname);
    static bool replaceHostInKnownHostsFile(QString hostname, QString keyType, QString key);
    static KnownHostsCheckResult checkKey(QString hostname, QString key);

private:
    static bool isHostInKnownHostLine(QString hostname, QString line);
};

#endif
