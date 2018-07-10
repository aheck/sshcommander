#ifndef FILEBROWSERAPPLET_H
#define FILEBROWSERAPPLET_H

#include <iostream>
#include <memory>

#include <QDir>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QHostInfo>
#include <QSplitter>
#include <QTreeView>
#include <QToolBar>

#include "FileTransferJob.h"
#include "FileTransfersApplet.h"
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

    // this applet needs to know the FileTransfersApplet
    void setFileTransfersApplet(FileTransfersApplet *applet);

protected slots:
    void expanded(const QModelIndex &index);
    void reloadData();
    void toggleLocalFileBrowser();
    void startDownload();
    void startUpload();

private:
    QAction *showLocalAction;
    QSplitter *splitter;
    QWidget *localFileBrowserWidget;
    QTreeView *localFileBrowser;
    QTreeView *remoteFileBrowser;
    QLabel *remoteHostnameLabel;
    QFileSystemModel *localFileSystemModel;
    SFTPFilesystemModel *remoteFileSystemModel;
    QToolBar *toolBar;
    QModelIndex lastIndexExpanded;
    FileTransfersApplet *fileTransfersApplet;
    bool firstShow;
};

#endif
