#include "SSHFilesystemManager.h"

#ifdef Q_OS_MACOS
#include <sys/mount.h>
#endif

SSHFSMountEntry::SSHFSMountEntry()
{
    this->termWidget = nullptr;
}

SSHFSMountEntry::~SSHFSMountEntry()
{
    this->unmount();
}

void SSHFSMountEntry::read(const QJsonObject &json)
{
    this->hostname = json["hostname"].toString();
    this->username = json["username"].toString();
    this->localDir = json["localDir"].toString();
    this->localDirCanonical = json["localDirCanonical"].toString();
    this->remoteDir = json["remoteDir"].toString();
    this->shortDescription = json["shortDescription"].toString();
    this->termWidget = nullptr;
}

void SSHFSMountEntry::write(QJsonObject &json) const
{
    json["hostname"] = this->hostname;
    json["username"] = this->username;
    json["localDir"] = this->localDir;
    json["localDirCanonical"] = this->localDirCanonical;
    json["remoteDir"] = this->remoteDir;
    json["shortDescription"] = this->shortDescription;
}

void SSHFSMountEntry::mount(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    const QStringList args = connEntry->generateSSHFSArgs(localDir, remoteDir);
    SSHTermWidget *termWidget = new SSHTermWidget(&args, connEntry, 0);
    termWidget->setAutoClose(true);
    termWidget->setShellProgram(ExternalProgramFinder::getSSHFSPath());
    termWidget->startShellProgram();
    this->termWidget = termWidget;
    termWidget->connect(termWidget, &SSHTermWidget::finished, &SSHFilesystemManager::getInstance(), &SSHFilesystemManager::sshfsTerminated);

    qDebug() << "SSHFS command: sshfs " << args.join(" ");
}

void SSHFSMountEntry::unmount()
{
    if (this->termWidget == nullptr) {
        return;
    }

    pid_t pid = this->termWidget->getShellPID();

    int result = 0;
    // We can't use waitpid here because the destructor of termWidget ultimately
    // calls the destructor of QProcess which doesn't like when we collect the
    // result of its child and then runs into a 30 second timeout.
    while (result == 0) {
        result = kill(pid, SIGTERM);
        QThread::msleep(10);
    }

    delete this->termWidget;
    this->termWidget = nullptr;
}

bool SSHFSMountEntry::isMounted()
{
    QString remote = this->username + "@" + this->hostname + ":" + remoteDir;
#ifdef Q_OS_LINUX
    QFile file("/proc/mounts");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QRegExp spaceCode("\\\\040");

    char lineBuffer[4096];

    while ((file.readLine(lineBuffer, sizeof(lineBuffer))) > 0) {
        QString line = QString(lineBuffer).trimmed();

        QStringList fields = line.split(" ");
        if (fields.length() < 3) {
            return false;
        }

        QString fsType = fields.at(2);

        if (fsType != "fuse.sshfs") {
            continue;
        }

        QString localDir = fields.at(1);
        localDir.replace(spaceCode, " ");
        if (localDir != this->localDirCanonical) {
            continue;
        }

        QString curRemote = fields.at(0);

        if (remote == curRemote) {
            return true;
        }
    }
#elif defined(Q_OS_MACOS)
    struct statfs *buf = NULL;

    int count = getmntinfo(&buf, 0);
    if (count == 0) {
        return false;
    }

    for (int i = 0; i < count; i++) {
        QString fstype = buf[i].f_fstypename;
        if (fstype != "osxfuse") {
            continue;
        }

        QString localDir = buf[i].f_mntonname;

        if (localDir != this->localDirCanonical) {
            continue;
        }

        QString curRemote = buf[i].f_mntfromname;

        if (remote == curRemote) {
            return true;
        }
    }
#endif
    return false;
}

SSHFilesystemManager& SSHFilesystemManager::getInstance()
{
    static SSHFilesystemManager sshFilesystemManager;
    return sshFilesystemManager;
}

SSHFilesystemManager::SSHFilesystemManager()
{
    connect(qApp, &QApplication::aboutToQuit, this, &SSHFilesystemManager::cleanup);

    this->restoreFromJson();
}

SSHFilesystemManager::~SSHFilesystemManager()
{
}

bool SSHFilesystemManager::saveToJson()
{
    QJsonDocument jsonDoc;
    QJsonObject jsonObject;

    for (auto const& cur : this->mountsByConnection) {
        QString connectionName = cur.first;

        QJsonArray mountsArray = QJsonArray();

        for (std::shared_ptr<SSHFSMountEntry> mountEntry : this->mountsByConnection[connectionName]) {
            QJsonObject curObj;
            mountEntry->write(curObj);
            mountsArray.append(curObj);
        }

        jsonObject[connectionName] = mountsArray;
    }

    jsonDoc.setObject(jsonObject);

    QDir jsonDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

    if (!jsonDir.exists()) {
        if (!jsonDir.mkpath(jsonDir.path())) {
            QMessageBox msgBox;
            msgBox.setText("Can't save SSHFS mounts. Failed to create directory '" + jsonDir.path() + "'");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            return false;
        }
    }

    QString jsonFilePath = jsonDir.filePath("mounts.json");
    QFile file(jsonFilePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Can't save SSHFS mounts. Failed to open file '" + jsonFilePath + "' for writing.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return false;
    }

    qint64 bytesWritten = file.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        QMessageBox msgBox;
        msgBox.setText("Can't save SSHFS mounts. Failed to write to file '" + jsonFilePath + "'");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    file.close();

    return true;
}

bool SSHFilesystemManager::restoreFromJson()
{
    QString filename = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath("mounts.json");
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    QByteArray fileContent(file.readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);

    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray connArray(jsonObj["connections"].toArray());

    for (const QString& connectionName : jsonObj.keys()) {
        QJsonArray connArray(jsonObj[connectionName].toArray());

        for (QJsonValue curValue : connArray) {
            std::shared_ptr<SSHFSMountEntry> mountEntry = std::make_shared<SSHFSMountEntry>();
            mountEntry->read(curValue.toObject());

            this->mountsByConnection[connectionName].push_back(mountEntry);
        }
    }

    file.close();

    return true;
}

int SSHFilesystemManager::countMounts(QString username, QString hostname)
{
    QString connectionId = username + "@" + hostname;
    return this->mountsByConnection[connectionId].size();
}

std::shared_ptr<SSHFSMountEntry> SSHFilesystemManager::getMountEntry(QString username, QString hostname, int row)
{
    const QString connectionId = username + "@" + hostname;

    auto mounts = this->mountsByConnection[connectionId];

    if (row < 0 || row > mounts.size() - 1) {
        return nullptr;
    }

    return mounts[row];
}

std::shared_ptr<SSHFSMountEntry> SSHFilesystemManager::getMountEntry(QString username, QString hostname, QString localDir, QString remoteDir)
{
    const QString connectionId = username + "@" + hostname;

    for (auto mountEntry : this->mountsByConnection[connectionId]) {
        if (mountEntry->localDir == localDir && mountEntry->remoteDir == remoteDir) {
            return mountEntry;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<SSHFSMountEntry>> SSHFilesystemManager::getMountEntries(QString username, QString hostname)
{
    QString connectionId = username + "@" + hostname;
    return this->mountsByConnection[connectionId];
}

void SSHFilesystemManager::createMountEntry(std::shared_ptr<SSHConnectionEntry> connEntry, QString localDir, QString remoteDir, QString shortDescription)
{
    std::shared_ptr<SSHFSMountEntry> mountEntry = std::make_shared<SSHFSMountEntry>();

    mountEntry->hostname = connEntry->hostname;
    mountEntry->username = connEntry->username;
    mountEntry->localDir = localDir;

    QFileInfo fileInfo(mountEntry->localDir);
    mountEntry->localDirCanonical = fileInfo.canonicalFilePath();

    mountEntry->remoteDir = remoteDir;
    mountEntry->shortDescription = shortDescription;

    const QString connection = connEntry->username + "@" + connEntry->hostname;
    this->mountsByConnection[connection].push_back(mountEntry);

    mountEntry->mount(connEntry);
}

bool SSHFilesystemManager::removeMountEntry(QString username, QString hostname, QString localDir, QString remoteDir)
{
    const QString connectionId = username + "@" + hostname;
    std::shared_ptr<SSHFSMountEntry> mountEntry = nullptr;

    int i = 0;
    for (auto cur : this->mountsByConnection[connectionId]) {
        if (cur->localDir == localDir && cur->remoteDir == remoteDir) {
            mountEntry = cur;
            break;
        }

        i++;
    }

    if (mountEntry == nullptr) {
        return false;
    }

    this->mountsByConnection[connectionId].erase(this->mountsByConnection[connectionId].begin() + i);
    //delete tunnel->termWidget;
    //tunnel->termWidget = nullptr;

    return true;
}

void SSHFilesystemManager::cleanup()
{
    this->saveToJson();

    for (auto const& cur : this->mountsByConnection) {
        this->mountsByConnection[cur.first].clear();
    }
}

void SSHFilesystemManager::sshfsTerminated(int exitStatus)
{
    QObject *term = sender();
    SSHTermWidget *termWidget = static_cast<SSHTermWidget*>(term);

    QString mountIdentifier;

    for (const auto& pair : this->mountsByConnection) {
        for (const auto &mountEntry : pair.second) {
            if (mountEntry == nullptr) {
                continue;
            }

            if (mountEntry->termWidget == termWidget) {
                mountIdentifier = " mounting " + mountEntry->remoteDir +
                    " from " + pair.first + " to local directory " + mountEntry->localDir;
            }
        }
    }

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("sshfs process" + mountIdentifier + " was terminated with exit status: " + QString::number(exitStatus));
    msgBox.exec();
}
