#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QHostInfo>
#include <QRegExp>
#include <QTextStream>

#include <memory>
#include <iostream>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "PseudoTerminal.h"
#include "SSHConnectionEntry.h"

class TestHelpers
{
public:
    static bool writeStringToFile(const QString &filename, QString &data);
    static QString readFileContents(const QString &filename);
    static QString genFileChecksum(const QString &filename, QCryptographicHash::Algorithm algo);

    static bool connectConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);

    static int scpFiles(std::shared_ptr<SSHConnectionEntry> connEntry, QString path);
    static QString sshSHA1Sum(std::shared_ptr<SSHConnectionEntry> connEntry, QString path);
};

#endif
