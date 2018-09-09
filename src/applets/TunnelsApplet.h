/*****************************************************************************
 *
 * TunnelsApplet is an applet which allows to user to create and manage SSH
 * tunnels to the remote machine.
 *
 ****************************************************************************/

#ifndef TUNNELSAPPLET_H
#define TUNNELSAPPLET_H

#include <iostream>
#include <memory>

#include <QHeaderView>
#include <QMenu>
#include <QTableView>
#include <QToolBar>

#include "TunnelManager.h"

#include "Applet.h"
#include "TunnelsItemModel.h"
#include "TunnelsNewDialog.h"

class TunnelsApplet : public Applet
{
    Q_OBJECT

public:
    TunnelsApplet();

    // the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

    void updateData();

public slots:
    void showCreateTunnelDialog();

protected slots:
    void createTunnel();
    void reloadData();
    void restartTunnel();
    void shutdownTunnel();
    void removeTunnel();
    void selectionChanged();
    void showContextMenu(QPoint pos);

protected:
    int getSelectedRow();

private:
    TunnelsNewDialog *newDialog;
    QToolBar *toolBar;
    QAction *restartAction;
    QAction *shutdownAction;
    QAction *deleteAction;
    QTableView *table;
    TunnelsItemModel *model;
    bool firstShow;
};

#endif
