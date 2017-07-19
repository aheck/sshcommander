/*****************************************************************************
 *
 * DetachedTerminalWindow is the window which is used when a terminal is
 * detached to its own window from a TabbedTerminalWidget
 *
 ****************************************************************************/

#ifndef DETACHEDTERMINALWINDOW_H
#define DETACHEDTERMINALWINDOW_H

#include <QCloseEvent>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

#include "TerminalContainer.h"

class DetachedTerminalWindow : public QWidget
{
    Q_OBJECT

signals:
    void tabReattachRequested(QUuid uuid);

public:
    DetachedTerminalWindow(QWidget *parent = 0);
    void setUuid(QUuid uuid);

protected:
    void closeEvent(QCloseEvent *event);

private:
    QUuid uuid;
};

#endif
