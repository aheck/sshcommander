#ifndef MACHINEINFOAPPLET_H
#define MACHINEINFOAPPLET_H

#include <memory>

#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "Applet.h"
#include "SSHConnectionManager.h"
#include "SSHConnectionEntry.h"

class MachineInfoApplet : public Applet
{
    Q_OBJECT

public:
    MachineInfoApplet();

    // implement the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

    void updateData();

public slots:
    void sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult);
    void updateKnownHostsData();

private slots:
    void removeHostFromKnownHosts();

private:
    QString getKnownHostsFilePath();
    bool isHostInKnownHostLine(QString hostname, QString line);
    bool isHostInKnownHostsFile();
    bool removeHostFromKnownHostsFile();

    QWidget *page;
    QLabel *valueHostname;
    QLabel *valueOperatingSystem;
    QLabel *valueDistro;
    QLabel *valueCpu;
    QLabel *valueMemory;
    QLabel *valueUsername;
    QLabel *valueSSHCommand;
    QLabel *valueSCPCommand;
    QLabel *valueSCPDirCommand;
    QLabel *valueKnownHostsFile;
    QLabel *valueFileExists;
    QLabel *valueKnownHostsEntryExists;
    QPushButton *removeHostButton;
    bool firstShow;
    QString knownHostsFilePath;
    QFileSystemWatcher fileWatcher;
};

#endif
