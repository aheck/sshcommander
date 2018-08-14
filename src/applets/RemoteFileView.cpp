#include "RemoteFileView.h"

RemoteFileView::RemoteFileView(QWidget *parent)
    : QTreeView(parent)
{

}

RemoteFileView::~RemoteFileView()
{

}

void RemoteFileView::dropEvent(QDropEvent *event)
{
    if (event->source() == this) {
        return;
    }

    QTreeView::dropEvent(event);
}
