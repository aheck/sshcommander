/*****************************************************************************
 *
 * ConnectionListWidget is the widget on the left-hand side of the main
 * window where the user can see and configure all of his SSH connections.
 *
 * This widget contains a QToolBar, a NewDialog (for editing connections)
 * a ConnectionListView (derived from QListView) and a pointer to a
 * SSHConnectionItemModel. The latter is the model displayed by the
 * ConnectionListView. It is created by the MainWindow and it contains
 * objects of type SSHConnectionEntry which describe the properties of a
 * single SSH connection like hostname, username and so on.
 *
 ****************************************************************************/

#ifndef CONNECTIONLISTWIDGET_H
#define CONNECTIONLISTWIDGET_H

#include <memory>

#include <QAction>
#include <QHelpEvent>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QToolBar>

#include "ConnectionListView.h"
#include "NewDialog.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class ConnectionListWidget : public QWidget
{
    Q_OBJECT

public:
    ConnectionListWidget(SSHConnectionItemModel *model);
    ~ConnectionListWidget();

    void selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry);
    QModelIndexList getSelection();
    void selectLast();
    std::shared_ptr<SSHConnectionEntry> getSelectedConnectionEntry();
    void selectFirstConnection();
    const QString getSelectedUsernameAndHost();
    void setAWSConsoleButton();
    void resetAWSConsoleButton();
    void disableAWSConsoleButton();
    void enableAWSConsoleButton();
    int connectionCount() const;

public slots:
    void editConnection();
    void moveConnection(int originRow, int targetRow);
    void removeSelectedConnection();
    void updateAWSInstances();

private slots:
    void showContextMenu(QPoint pos);
    void selectionChanged(const QItemSelection &selected,
    const QItemSelection &deselected);
    void awsConsoleToggled(bool checked);

signals:
    // the user requested a dialog to create a new connection
    void newDialogRequested();

    // a connection was removed by the user
    void connectionRemoved(std::shared_ptr<SSHConnectionEntry> connEntry);

    // the user selected another connection
    void connectionChanged(int row);

    void toggleAwsConsole(bool show);

    // the user moved a connection in the list widget
    void connectionMoved(int originRow, int targetRow);

private:
    QToolBar *toolBar;
    NewDialog *editDialog;
    ConnectionListView *listView;
    SSHConnectionItemModel *model;
    QAction *editAction;
    QAction *deleteAction;
    QAction *awsConsoleAction;
};

#endif
