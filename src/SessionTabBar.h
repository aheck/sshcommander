#ifndef SESSIONTABBAR_H
#define SESSIONTABBAR_H

#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

#include "CustomTabBar.h"

class SessionTabBar : public CustomTabBar
{
    Q_OBJECT

public:
    SessionTabBar(QWidget *parent = 0);
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void tabDetachRequested(int index);

private:
    QPoint startPos;
    bool dragging;
};

#endif
