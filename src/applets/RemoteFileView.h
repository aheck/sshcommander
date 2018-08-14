/*****************************************************************************
 *
 * RemoteFileView is a QTreeView with some added drag and drop code for
 * remote SFTP file browsing via SFTPFilesystemModel.
 *
 ****************************************************************************/

#ifndef REMOTEFILEVIEW_H
#define REMOTEFILEVIEW_H

#include <iostream>

#include <QDropEvent>
#include <QMimeData>
#include <QTreeView>

class RemoteFileView : public QTreeView
{
    Q_OBJECT

public:
    explicit RemoteFileView(QWidget *parent = 0);
    ~RemoteFileView();

    void dropEvent(QDropEvent *event) override;
};

#endif
