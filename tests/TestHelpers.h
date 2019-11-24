#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QCryptographicHash>
#include <QDateTime>
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
    static bool writeStringToFile(const QString &filename, const QString &data);
    static QString readFileContents(const QString &filename);
    static QString genFileChecksum(const QString &filename, QCryptographicHash::Algorithm algo);

    static std::shared_ptr<SSHConnectionEntry> buildConnEntry(int sshPort);
    static bool connectConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);

    static int scpFiles(std::shared_ptr<SSHConnectionEntry> connEntry, const QString &path);
    static QString sshSHA1Sum(std::shared_ptr<SSHConnectionEntry> connEntry, const QString &path);

private:
    static bool enterPassword(PseudoTerminal &term, const QString &password, int timeoutMsecs = 30000);
};

#endif
