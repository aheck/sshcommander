#ifndef CONNECTIONLISTVIEW_H
#define CONNECTIONLISTVIEW_H

#include <memory>

#include <QHelpEvent>
#include <QListView>
#include <QToolTip>

#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class ConnectionListView : public QListView
{
    Q_OBJECT

public:
    ConnectionListView();

    bool event(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void connectionMoved(int originRow, int targetRow);

private:
    int draggedRow;
};

#endif
