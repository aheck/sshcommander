#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

class CustomTabBar : public QTabBar
{
    Q_OBJECT

public:
    CustomTabBar(QWidget *parent = 0);
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
