/*****************************************************************************
 *
 * SSHFilesystemManager is the singleton that manages all sshfs mounts
 * created by SSH Commander.
 *
 * It is used by all classes responsible for creating, listing and destroying
 * sshfs mounts.
 *
 ****************************************************************************/

#ifndef SSHFILESYSTEMMANAGER_H
#define SSHFILESYSTEMMANAGER_H

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <map>
#include <memory>
#include <vector>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QThread>

#include "ExternalProgramFinder.h"
#include "SSHConnectionEntry.h"


struct SSHFSMountEntry
{
    QString hostname;
    QString username;
    QString localDir;
    QString localDirCanonical;
    QString remoteDir;
    QString shortDescription;
    SSHTermWidget *termWidget;

    SSHFSMountEntry();
    ~SSHFSMountEntry();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void mount(std::shared_ptr<SSHConnectionEntry>);
    void unmount();
    bool isMounted();
};

class SSHFilesystemManager : public QObject
{
    Q_OBJECT

public:
    static SSHFilesystemManager& getInstance();

    SSHFilesystemManager(SSHFilesystemManager const &other) = delete;
    void operator=(SSHFilesystemManager const &) = delete;

    bool saveToJson();
    bool restoreFromJson();

    int countMounts(QString username, QString hostname);
    std::shared_ptr<SSHFSMountEntry> getMountEntry(QString username, QString hostname, int row);
    std::shared_ptr<SSHFSMountEntry> getMountEntry(QString username, QString hostname, QString localDir, QString remoteDir);
    std::vector<std::shared_ptr<SSHFSMountEntry>> getMountEntries(QString username, QString hostname);

    void createMountEntry(std::shared_ptr<SSHConnectionEntry> connEntry, QString localDir, QString remoteDir, QString shortDescription);
    bool removeMountEntry(QString username, QString hostname, QString localDir, QString remoteDir);

public slots:
    void cleanup();
    void sshfsTerminated(int exitStatus);

private:
    SSHFilesystemManager();
    ~SSHFilesystemManager();

    std::map<const QString, std::vector<std::shared_ptr<SSHFSMountEntry>>> mountsByConnection;
};

#endif
