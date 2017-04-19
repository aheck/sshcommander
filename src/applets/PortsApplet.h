#ifndef PORTSAPPLET_H
#define PORTSAPPLET_H

#include <iostream>
#include <memory>

#include <QHeaderView>
#include <QTableView>
#include <QToolBar>

#include "Applet.h"
#include "PortsItemModel.h"
#include "SSHConnectionManager.h"

class PortsApplet : public Applet
{
    Q_OBJECT

public:
    PortsApplet();

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
    PortsItemModel *model;
    bool firstShow;
};

#endif
