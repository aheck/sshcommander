#ifndef PROCESSESAPPLET_H
#define PROCESSESAPPLET_H

#include <iostream>
#include <memory>

#include <QHeaderView>
#include <QTableView>
#include <QToolBar>

#include "Applet.h"
#include "ProcessesItemModel.h"
#include "SSHConnectionManager.h"

class ProcessesApplet : public Applet
{
    Q_OBJECT

public:
    ProcessesApplet();

    // the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

    void updateData();

public slots:
    void sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult);
    void reloadData();

private:
    QToolBar *toolBar;
    QTableView *table;
    ProcessesItemModel *model;
    bool firstShow;
};

#endif
