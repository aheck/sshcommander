#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

#include <memory>

#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "DisabledWidget.h"
#include "SSHConnectionEntry.h"

class MachineInfoWidget : public QWidget
{
    Q_OBJECT

public:
    MachineInfoWidget();

    void setMachineEnabled(bool enabled);
    void updateData(std::shared_ptr<SSHConnectionEntry> connEntry);

private:
    bool enabled;
    QWidget *page;
    DisabledWidget *disabledWidget;
    QStackedWidget *widgetStack;
    QGridLayout *gridLayout;
    QLabel *labelHostname;
    QLabel *valueHostname;
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
