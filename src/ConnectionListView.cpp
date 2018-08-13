#include "ConnectionListView.h"

ConnectionListView::ConnectionListView()
{
    this->setUniformItemSizes(true);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setDragEnabled(true);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDefaultDropAction(Qt::MoveAction);

    this->setStyleSheet(
            "QListView::item { border: 0; padding: 0.75em; }\n"
            "QListView::item:selected { color: #ffffff; background: "
            "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #307ece, stop: 1 #73b6fa); }\n"
            "QListView::item:selected:hover { color: #ffffff; background: "
            "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #307ece, stop: 1 #73b6fa); }\n"
            "QListView::item:hover { color: #000000; background: "
            "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #9fccfa, stop: 1 #c1e0ff); }\n");
    this->setFocusPolicy(Qt::NoFocus);
}

bool ConnectionListView::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QModelIndex index = this->indexAt(helpEvent->pos());

        if (index.isValid()) {
            QAbstractItemModel *abstractModel = this->model();
            SSHConnectionItemModel *model = static_cast<SSHConnectionItemModel *>(abstractModel);
            auto connEntry = model->getConnEntry(index.row());

            QString text = "<b>SSH</b><table>";
            text += "<tr><td>Hostname: </td><td>" + connEntry->hostname + "</td></tr>";
            text += "<tr><td>Username: </td><td>" + connEntry->username + "</td></tr>";
            if (!connEntry->sshkey.isEmpty()) {
                text += "<tr><td>SSH Key: </td><td>" + connEntry->sshkey + "</td></tr>";
            }

            if (!connEntry->hopHosts.isEmpty()) {
                text += "<tr><td>Hop Hosts: </td><td>";

                for (int i = 0; i < connEntry->hopHosts.count(); i++) {
                    const QString hop = connEntry->hopHosts.at(i);
                    text += hop;

                    // all lines but the last one end with a line break
                    if (i < connEntry->hopHosts.count() - 1) {
                        text += "<br>";
                    }
                }

                text += "</td></tr>";
            }
            text += "</table>";

            if (connEntry->isAwsInstance) {
                text += "<br><br><b>AWS</b><table>";
                text += "<tr><td>Instance ID: </td><td>" + connEntry->awsInstance->id + "</td></tr>";
                if (!connEntry->awsInstance->name.isEmpty()) {
                    text += "<tr><td>Instance Name: </td><td>" + connEntry->awsInstance->name + "</td></tr>";
                }

                QString statusColor = "goldenrod";
                if (connEntry->awsInstance->status == "terminated") {
                    statusColor = "red";
                } else if (connEntry->awsInstance->status == "running") {
                    statusColor = "green";
                }

                text += "<tr><td>Status: </td><td style=\"font-weight:bold; color: " + statusColor + ";\">" + connEntry->awsInstance->status + "</td></tr>";
                text += "<tr><td>Region: </td><td>" + connEntry->awsInstance->region + "</td></tr>";
                text += "<tr><td>Availability Zone: </td><td>" + connEntry->awsInstance->availabilityZone + "</td></tr>";
                text += "<tr><td>VPC: </td><td>" + connEntry->awsInstance->formattedVpc() + "</td></tr>";
                text += "<tr><td>Subnet: </td><td>" + connEntry->awsInstance->formattedSubnet() + "</td></tr>";
                text += "<tr><td>Keyname: </td><td>" + connEntry->awsInstance->keyname + "</td></tr>";
                text += "<tr><td>Instance Type: </td><td>" + connEntry->awsInstance->type + "</td></tr>";
                if (!connEntry->awsInstance->cfStackName.isEmpty()) {
                    text += "<tr><td>CF Stack: </td><td>" + connEntry->awsInstance->cfStackName + "</td></tr>";
                }
                text += "</table>";
            }

            QToolTip::showText(helpEvent->globalPos(), text);
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }

    return QListView::event(event);
}

void ConnectionListView::dragEnterEvent(QDragEnterEvent *event)
{
    QModelIndexList indexes = this->selectedIndexes();
    this->draggedRow = indexes.first().row();
    event->accept();
}

void ConnectionListView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->ignore();
        return;
    }

    event->setDropAction(Qt::MoveAction);
    event->accept();
    QListView::dragMoveEvent(event);
}

void ConnectionListView::dropEvent(QDropEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    int originRow = this->draggedRow;
    int targetRow = index.row();

    if (event->source() != this) {
        event->ignore();
        return;
    }

    // append when targetRow < 0
    if (targetRow < 0) {
        targetRow = this->model()->rowCount() - 1;
    }

    if (originRow == targetRow) {
        event->ignore();
        return;
    }

    if (originRow < 0) {
        event->ignore();
        return;
    }

    if (originRow >= this->model()->rowCount() || targetRow >= this->model()->rowCount()) {
        event->ignore();
        return;
    }

    static_cast<SSHConnectionItemModel*>(this->model())->moveConnectionEntry(originRow, targetRow);

    this->setCurrentIndex(this->model()->index(targetRow, 0));
    event->accept();
    emit connectionMoved(originRow, targetRow);
}
