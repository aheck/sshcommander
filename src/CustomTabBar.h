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
    void setTabChanged(int index);

private slots:
    void currentChanged(int index);
};

#endif
