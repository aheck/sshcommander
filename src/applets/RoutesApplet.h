#ifndef ROUTESAPPLET_H
#define ROUTESAPPLET_H

#include <iostream>
#include <memory>

#include <QHeaderView>
#include <QTableView>
#include <QToolBar>

#include "Applet.h"
#include "RoutesItemModel.h"
#include "SSHConnectionManager.h"

class RoutesApplet : public Applet
{
    Q_OBJECT

public:
    RoutesApplet();

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
    RoutesItemModel *model;
    bool firstShow;
};

#endif
