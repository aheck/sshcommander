#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

#include <memory>

#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "SSHConnectionManager.h"
#include "DisabledWidget.h"
#include "SSHConnectionEntry.h"

class MachineInfoWidget : public QWidget
{
    Q_OBJECT

public:
    MachineInfoWidget();

    void setMachineEnabled(bool enabled);
    void updateData(std::shared_ptr<SSHConnectionEntry> connEntry);

public slots:
    void sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult);

private:
    bool enabled;
    std::shared_ptr<SSHConnectionEntry> connEntry;
    QWidget *page;
    DisabledWidget *disabledWidget;
    QStackedWidget *widgetStack;
    QGridLayout *gridLayout;
    QLabel *labelHostname;
    QLabel *valueHostname;
    QLabel *labelOperatingSystem;
    QLabel *valueOperatingSystem;
    QLabel *labelDistro;
    QLabel *valueDistro;
    QLabel *labelCpu;
    QLabel *valueCpu;
    QLabel *labelMemory;
    QLabel *valueMemory;
    QLabel *labelUsername;
    QLabel *valueUsername;
    QLabel *labelSSHCommand;
    QLabel *valueSSHCommand;
    QLabel *labelSCPCommand;
    QLabel *valueSCPCommand;
    QLabel *labelSCPDirCommand;
    QLabel *valueSCPDirCommand;
};

#endif
