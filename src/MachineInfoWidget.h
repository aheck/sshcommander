#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

#include <memory>

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

class MachineInfoWidget : public Applet
{
    Q_OBJECT

public:
    MachineInfoWidget();

    // implement the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;

    void updateData(std::shared_ptr<SSHConnectionEntry> connEntry);

public slots:
    void sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult);

private:
    std::shared_ptr<SSHConnectionEntry> connEntry;
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
};

#endif
