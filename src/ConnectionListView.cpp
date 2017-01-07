#include "ConnectionListView.h"

ConnectionListView::ConnectionListView()
{
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
                text += "<tr><td>Region: </td><td>" + connEntry->awsInstance->region + "</td></tr>";
                text += "<tr><td>Availability Zone: </td><td>" + connEntry->awsInstance->availabilityZone + "</td></tr>";
                text += "<tr><td>VPC: </td><td>" + connEntry->awsInstance->formattedVPC() + "</td></tr>";
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
