#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

#include <memory>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
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
