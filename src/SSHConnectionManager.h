/*****************************************************************************
 *
 * SSHConnectionManager is a singleton that manages libssh2 connections to
 * the hosts.
 *
 * The terminal connections are implemented by running the system's ssh
 * program in a terminal emulator. This is nice for driving terminals and
 * having them isolated from each other as well as from the rest of
 * the program. But it doesn't allow code to talk to the instances.
 *
 * Therefore this class provides direct and programmatic SSH access to all
 * of the users remote machines. It is usually used by applets to implement
 * all kinds of automatization features.
 *
 * Parts of this class are based on example code from libssh2
 *
 ****************************************************************************/

#ifndef SSHCONNECTIONMANAGER_H
#define SSHCONNECTIONMANAGER_H

#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <QHostInfo>
#include <QMetaObject>
#include <QString>

#include "DirEntry.h"
#include "SSHConnection.h"
#include "SSHConnectionEntry.h"

#include <QMetaType>

class RemoteCmdResult
{
public:
    uint64_t requestId;
    bool isSuccess;
    QString errorString;
    int statusCode;
    QString command;
    QString resultString;
};

// declare metatypes for QMetaObject::invokeMethod
Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_METATYPE(std::shared_ptr<RemoteCmdResult>)

class SSHConnectionManager
{
public:
    ~SSHConnectionManager();
    SSHConnectionManager(SSHConnectionManager const &other) = delete;
    void operator=(SSHConnectionManager const &) = delete;

    static SSHConnectionManager& getInstance();

    uint64_t executeRemoteCmd(std::shared_ptr<SSHConnectionEntry> connEntry,
            QString cmd, QObject *slotObject, const char *slot);
    std::vector<std::shared_ptr<DirEntry>> readDirectory(std::shared_ptr<SSHConnectionEntry>, QString dir, bool onlyDirs);

private:
    SSHConnectionManager();

    std::shared_ptr<SSHConnection> createSSHConnection(std::shared_ptr<SSHConnectionEntry> connEntry);
    uint64_t generateRequestId();
    std::shared_ptr<RemoteCmdResult> doExecuteRemoteCmd(std::shared_ptr<SSHConnection>, QString cmd);
    int waitsocket(std::shared_ptr<SSHConnection> conn);
    std::vector<std::shared_ptr<DirEntry>> doReadDirectory(std::shared_ptr<SSHConnectionEntry> connEntry, QString dir, bool onlyDirs);

    std::mutex requestIdMutex;
    uint64_t nextRequestId;
};

#endif
