/*****************************************************************************
 *
 * ConnectionListWidget is the widget on the left-hand side of the main
 * window where the user can see and configure all of his SSH connections.
 *
 * This widget contains a QToolBar, a ConnectionListView (derived from
 * QListView) and a pointer to a SSHConnectionItemModel. The latter is the
 * model displayed by the ConnectionListView. It is created by the MainWindow
 * and it contains objects of type SSHConnectionEntry which describe the
 * properties of a single SSH connection like hostname, username and so on.
 *
 ****************************************************************************/

#ifndef CONNECTIONLISTWIDGET_H
#define CONNECTIONLISTWIDGET_H

#include <memory>

#include <QAction>
#include <QHelpEvent>
#include <QMenu>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QToolBar>

#include "ConnectionListView.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class ConnectionListWidget : public QWidget
{
    Q_OBJECT

public:
    ConnectionListWidget(SSHConnectionItemModel *model);

    void selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry);
    QModelIndexList getSelection();
    void selectLast();

private slots:
    void showContextMenu(QPoint pos);

signals:
    void showNewDialog();
    void editConnection();
    void removeConnection();
    void changeConnection(const QItemSelection &selected,
            const QItemSelection &deselected);

private:
    QToolBar *toolBar;
    ConnectionListView *listView;
    SSHConnectionItemModel *model;
};

#endif
