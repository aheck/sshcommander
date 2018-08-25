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

#include <QCoreApplication>
#include <QHostInfo>
#include <QMessageBox>
#include <QMetaObject>
#include <QMetaType>
#include <QMutex>
#include <QString>
#include <QUuid>

#include "DirEntry.h"
#include "FileTransferJob.h"
#include "FileTransferWorker.h"
#include "KnownHosts.h"
#include "SSHConnection.h"
#include "SSHConnectionEntry.h"
#include "Util.h"

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

class SSHConnectionManager : QObject
{
    Q_OBJECT
public:
    friend class FileTransferWorker;

    ~SSHConnectionManager();
    SSHConnectionManager(SSHConnectionManager const &other) = delete;
    void operator=(SSHConnectionManager const &) = delete;

    static SSHConnectionManager& getInstance();

    uint64_t executeRemoteCmd(std::shared_ptr<SSHConnectionEntry> connEntry,
            QString cmd, QObject *slotObject, const char *slot);
    std::vector<std::shared_ptr<DirEntry>> readDirectory(std::shared_ptr<SSHConnectionEntry>, QString dir, bool onlyDirs);
    void addFileTransferJob(std::shared_ptr<FileTransferJob> job);

    static int waitsocket(std::shared_ptr<SSHConnection> conn, unsigned int timeoutMs = 2000);
    static int waitsocketWrapper(std::shared_ptr<SSHConnection> conn, unsigned int timeoutSec = 2);

    // access methods for file transfer jobs
    int countFileTransferJobs(QString connectionId);
    std::shared_ptr<FileTransferJob> getFileTransferJob(QString connectionId, int row);
    bool restartFileTransferJob(QString connectionId, int row);
    bool removeFileTransferJob(QString connectionId, int row);
    int getFileTransferJobRowByUuid(QString connectionId, QUuid uuid);

    QByteArray getInteractiveAuthPassword();

public slots:
    // this slot is used by FileTransferWorkers (which run in separate
    // threads) to communicate with the user by creating a box in the
    // main thread
    void askToOverwriteFile(QString title, QString message, QString infoText);

    // same for known host checking
    void askToAddKey(QString fingerprint);
    void askToReplaceKey(QString fingerprint);

private:
    SSHConnectionManager();

    std::shared_ptr<SSHConnection> createSSHConnection(std::shared_ptr<SSHConnectionEntry> connEntry);
    uint64_t generateRequestId();
    std::shared_ptr<RemoteCmdResult> doExecuteRemoteCmd(std::shared_ptr<SSHConnection>, QString cmd);
    std::vector<std::shared_ptr<DirEntry>> doReadDirectory(std::shared_ptr<SSHConnection> conn, QString dir, bool onlyDirs);
    void executeFileTransfer(std::shared_ptr<FileTransferJob> job);

    std::map<QString, std::vector<std::shared_ptr<FileTransferJob>>> fileTransferJobs;
    std::mutex requestIdMutex;
    uint64_t nextRequestId;

    QMutex interactiveAuthMutex;
    QByteArray interactiveAuthPassword;

    std::atomic<int> addKeyAnswer;
    std::atomic<int> replaceKeyAnswer;
};

#endif
