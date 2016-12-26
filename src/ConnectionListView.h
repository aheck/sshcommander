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
};

#endif
