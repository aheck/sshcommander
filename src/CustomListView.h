#ifndef CUSTOMLISTVIEW_H
#define CUSTOMLISTVIEW_H

#include <memory>

#include <QHelpEvent>
#include <QListView>
#include <QToolTip>

#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class CustomListView : public QListView
{
    Q_OBJECT

public:
    CustomListView();

    bool event(QEvent *event) override;
};

#endif
