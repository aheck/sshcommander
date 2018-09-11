/*****************************************************************************
 *
 * FileBrowserApplet is an SFTP file browser allowing the user to browse the
 * filesystem of the remote machine.
 *
 * It also provides a toggleable local file browser and file transfers can
 * be started by drag and drop.
 *
 ****************************************************************************/

#ifndef FILEBROWSERAPPLET_H
#define FILEBROWSERAPPLET_H

#include <iostream>
#include <memory>

#include <QDir>
#include <QHeaderView>
#include <QHostInfo>
#include <QSplitter>
#include <QTreeView>
#include <QToolBar>

#include "../Applet.h"
#include "../NotificationManager.h"

#include "FileSystemModel.h"
#include "FileTransferJob.h"
#include "FileTransfersApplet.h"
#include "RemoteFileView.h"
#include "SSHConnectionManager.h"
#include "SFTPFilesystemModel.h"

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

public slots:
    void fileUploadRequested(QStringList files, QString targetPath);
    void fileDownloadRequested(QStringList files, QString targetPath);

protected slots:
    void expanded(const QModelIndex &index);
    void reloadData();
    void toggleLocalFileBrowser();

private:
    QAction *showLocalAction;
    QSplitter *splitter;
    QWidget *localFileBrowserWidget;
    QTreeView *localFileBrowser;
    RemoteFileView *remoteFileBrowser;
    QLabel *remoteHostnameLabel;
    FileSystemModel *localFileSystemModel;
    SFTPFilesystemModel *remoteFileSystemModel;
    QToolBar *toolBar;
    QModelIndex lastIndexExpanded;
    FileTransfersApplet *fileTransfersApplet;
    bool firstShow;
    bool localFileBrowserFirstShow;
};

#endif
