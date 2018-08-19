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

class KnownHosts
{
public:
    static const QString getSSHClientConfigFilePath();
    static bool isPasswordHashingEnabled();
    static QString getKnownHostsFilePath();
    static bool isHostInKnownHostsFile(QString hostname);
    static bool removeHostFromKnownHostsFile(QString hostname);

private:
    static bool isHostInKnownHostLine(QString hostname, QString line);
};

#endif
