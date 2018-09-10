/*****************************************************************************
 *
 * SSHFilesystemApplet is an applet which allows to mount directories on a
 * remote machine via sshfs such that they appear like local directories.
 *
 ****************************************************************************/

#ifndef SSHFILESYSTEMAPPLET_H
#define SSHFILESYSTEMAPPLET_H

#include <iostream>
#include <memory>

#include <QHeaderView>
#include <QDesktopServices>
#include <QMenu>
#include <QTableView>
#include <QToolBar>
#include <QItemSelection>

#include "SSHConnectionManager.h"
#include "SSHFilesystemManager.h"

#include "SSHFilesystemItemModel.h"
#include "SSHFilesystemNewDialog.h"

#include "../Applet.h"
#include "../ExternalProgramFinder.h"
#include "../DisabledWidget.h"

class SSHFilesystemApplet : public Applet
{
    Q_OBJECT

public:
    SSHFilesystemApplet();

    // the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

    void updateData();

public slots:
    void reloadData();
    void showNewDialog();

protected slots:
    void selectionChanged();
    void createNewMountEntry();
    void openDirectory();
    void mountMountEntry();
    void unmountMountEntry();
    void removeMountEntry();
    void showContextMenu(QPoint pos);

protected:
    int getSelectedRow();

private:
    bool sshfsNotInstalled;
    SSHFilesystemNewDialog *newDialog;
    QToolBar *toolBar;
    QAction *openAction;
    QAction *mountAction;
    QAction *unmountAction;
    QAction *deleteAction;
    QTableView *table;
    SSHFilesystemItemModel *model;
    bool firstShow;
};

#endif
