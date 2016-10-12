#include <QTabWidget>

#include "CustomTabBar.h"

#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit CustomTabWidget(QWidget *parent = 0);
};

#endif
