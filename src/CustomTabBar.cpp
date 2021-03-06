#include "CustomTabBar.h"

CustomTabBar::CustomTabBar(QWidget *parent) :
    QTabBar(parent)
{
    connect(this, &CustomTabBar::currentChanged, this, &CustomTabBar::currentChanged);

#ifdef Q_OS_MACOS
    this->setDocumentMode(true);
#endif
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
