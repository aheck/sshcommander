#ifndef FILETRANSFERSAPPLET_H
#define FILETRANSFERSAPPLET_H

#include <iostream>
#include <memory>

#include <QDir>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QHostInfo>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>
#include <QToolBar>

#include "Applet.h"
#include "FileTransferJob.h"
#include "FileTransfersItemModel.h"
#include "SSHConnectionManager.h"
#include "SFTPFilesystemModel.h"

class FileTransfersApplet : public Applet
{
    Q_OBJECT

public:
    FileTransfersApplet();
    ~FileTransfersApplet();

    // the Applet interface
    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;
    virtual void onShow() override;

protected slots:
    void reloadData();

private:
    QTableView *table;
    FileTransfersItemModel *model;
    QToolBar *toolBar;
    bool firstShow;
};

#endif
