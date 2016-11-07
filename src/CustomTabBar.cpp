#include "CustomTabBar.h"

#include <QInputDialog>
#include <QMouseEvent>

CustomTabBar::CustomTabBar(QWidget *parent) :
    QTabBar(parent)
{
}
 
void CustomTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    int tabIndex = this->currentIndex();
    bool success = true;

    QString name = QInputDialog::getText(this, tr("Change Session Name"),
                tr("Enter new Session Name"), QLineEdit::Normal,
                this->tabText(tabIndex), &success);
 
    if (success) {
        QString newName = name.trimmed();
        if (!newName.isEmpty()) {
            setTabText(tabIndex, newName);
        }
    }
}
