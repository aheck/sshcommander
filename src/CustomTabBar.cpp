#include "CustomTabBar.h"

CustomTabBar::CustomTabBar(QWidget *parent) :
    QTabBar(parent)
{
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
}

void CustomTabBar::setTabChanged(int index)
{
    QColor color("#0000ff");
    this->setTabTextColor(index, color);
}

void CustomTabBar::currentChanged(int index)
{
    QColor color("#000000");
    this->setTabTextColor(index, color);
}
