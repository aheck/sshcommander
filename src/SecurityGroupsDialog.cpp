#include "SecurityGroupsDialog.h"

SecurityGroupsDialog::SecurityGroupsDialog()
{
    this->setMinimumWidth(300);
    QVBoxLayout *layout = new QVBoxLayout();

    this->list = new QListWidget();
    layout->addWidget(this->list);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(closeButton);

    this->setLayout(layout);
}

void SecurityGroupsDialog::showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance)
{
    QString title;
    if (instance->name.isEmpty()) {
        title = QString("Security Groups of instance %1").arg(instance->id);
    } else {
        title = QString("Security Groups of instance '%1' (%2)").arg(instance->name).arg(instance->id);
    }

    this->setWindowTitle(title);

    this->list->clear();

    if (instance->securityGroups.count() > 0) {
        QList<QString> groupIds;
        for (AWSSecurityGroup sg : instance->securityGroups) {
            groupIds.append(sg.id);
        }
        connector->describeSecurityGroups(groupIds);
    } else {
        this->list->addItem("No Security Groups");
    }

    this->exec();
}

void SecurityGroupsDialog::updateData(std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups)
{
    for (std::shared_ptr<AWSSecurityGroup> sg : securityGroups) {
        QListWidgetItem *item = new QListWidgetItem(QString("SG: %1 (%2)").arg(sg->name).arg(sg->id),
                this->list, QListWidgetItem::Type);
        QFont font = item->font();
        font.setBold(true);
        item->setFont(font);
        item->setToolTip(sg->description);
        this->list->addItem(item);
        this->list->addItem("Ingress");
        for (AWSIngressPermission perm : sg->ingressPermissions) {
            QString item = perm.ipProtocol + ": " + perm.fromPort + "-" + perm.toPort + " ";

            int i = 0;
            for (QString cidr : perm.cidrs) {
                if (i != 0) {
                    item += ",";
                }
                item += cidr;
                i++;
            }

            this->list->addItem(item);
        }

        this->list->addItem("Egress");
        for (AWSEgressPermission perm : sg->egressPermissions) {
            QString item = perm.ipProtocol + ": " + perm.fromPort + "-" + perm.toPort + " ";

            int i = 0;
            for (QString cidr : perm.cidrs) {
                if (i != 0) {
                    item += ",";
                }
                item += cidr;
                i++;
            }

            this->list->addItem(item);
        }
    }
}
