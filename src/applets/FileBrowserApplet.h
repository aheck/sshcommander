#ifndef FILEBROWSERAPPLET_H
#define FILEBROWSERAPPLET_H

#include <iostream>
#include <memory>

#include <QDir>
#include <QFileSystemModel>
#include <QSplitter>
#include <QTreeView>
#include <QToolBar>

#include "SSHConnectionManager.h"
#include "SFTPFilesystemModel.h"

#include "Applet.h"

class FileBrowserApplet : public Applet
{
    Q_OBJECT

public:
    FileBrowserApplet();
    ~FileBrowserApplet();

    // the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

protected slots:
    void toggleLocalFileBrowser();

private:
    QAction *showLocalAction;
    QSplitter *splitter;
    QTreeView *localFileBrowser;
    QTreeView *remoteFileBrowser;
    QFileSystemModel *localFileSystemModel;
    SFTPFilesystemModel *remoteFileSystemModel;
    QToolBar *toolBar;
    bool firstShow;
};

#endif
