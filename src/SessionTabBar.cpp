#include "SessionTabBar.h"

#include <QInputDialog>

SessionTabBar::SessionTabBar(QWidget *parent) :
    CustomTabBar(parent)
{
    this->dragging = false;
}

void SessionTabBar::mouseDoubleClickEvent(QMouseEvent *event)
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

void SessionTabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->startPos = event->pos();
        this->dragging = true;
    }

    QTabBar::mousePressEvent(event);
}

void SessionTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (this->dragging) {
            if (qAbs(this->startPos.y() - event->pos().y()) >= 50) {
                emit tabDetachRequested(this->currentIndex());
            }
        }

        this->startPos = QPoint();
        this->dragging = false;
    }

    QTabBar::mouseReleaseEvent(event);
}
