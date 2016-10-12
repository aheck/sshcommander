#include "CustomTabWidget.h"

CustomTabWidget::CustomTabWidget(QWidget *parent) :
        QTabWidget(parent)
{
    CustomTabBar* tabBar = new CustomTabBar();
    this->setTabBar(tabBar);
}
