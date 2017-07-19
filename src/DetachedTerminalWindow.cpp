#include "DetachedTerminalWindow.h"

DetachedTerminalWindow::DetachedTerminalWindow(QWidget *parent)
{
    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->resize(1024, 768);
}

void DetachedTerminalWindow::setUuid(QUuid uuid)
{
    this->uuid = uuid;
}

void DetachedTerminalWindow::closeEvent(QCloseEvent *event)
{
    emit tabReattachRequested(this->uuid);
    event->ignore();
}
