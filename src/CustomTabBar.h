#include <QTabBar>

#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

class CustomTabBar : public QTabBar
{
    Q_OBJECT

public:
    CustomTabBar(QWidget *parent = 0);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif