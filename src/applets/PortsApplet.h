/*****************************************************************************
 *
 * PortsApplet is an applet which shows all listening TCP ports on the remote
 * machine and also allows to create SSH tunnels to those services.
 *
 ****************************************************************************/

#ifndef PORTSAPPLET_H
#define PORTSAPPLET_H

#include <iostream>
#include <memory>

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QTableView>
#include <QToolBar>

#include "Applet.h"
#include "PortsItemModel.h"
#include "PortsNewTunnelDialog.h"
#include "RichTextDelegate.h"
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
    int getSelectedRow();

public slots:
    void sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult);
    void reloadData();

protected slots:
    void showContextMenu(QPoint pos);
    void showNewTunnelDialog();
    void createTunnel();

private:
    QToolBar *toolBar;
    QTableView *table;
    PortsItemModel *model;
    PortsNewTunnelDialog *newDialog;
    bool firstShow;
};

#endif
