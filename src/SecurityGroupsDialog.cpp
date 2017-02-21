#include "SecurityGroupsDialog.h"

SecurityGroupsDialog::SecurityGroupsDialog()
{
    this->setMinimumWidth(500);
    this->setMinimumHeight(300);
    QVBoxLayout *layout = new QVBoxLayout();

    this->table = new QTableWidget(this);
    QStringList columnNames = {"Protocol", "Start Port", "End Port", "Networks"};
    this->table->setColumnCount(4);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->verticalHeader()->setVisible(false);
    this->table->setHorizontalHeaderLabels(columnNames);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    layout->addWidget(this->table);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

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

    this->table->setRowCount(0);
    this->table->clearContents();

    if (instance->securityGroups.count() > 0) {
        QList<QString> groupIds;
        for (AWSSecurityGroup sg : instance->securityGroups) {
            groupIds.append(sg.id);
        }
        connector->describeSecurityGroups(groupIds);
    } else {
        this->table->setRowCount(1);
        this->table->setItem(0, 0, new QTableWidgetItem(tr("No Security Groups")));
        this->table->setSpan(0, 0, 1, 4);
    }

    this->exec();
}

void SecurityGroupsDialog::updateData(std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups)
{
    int row = 0;

    for (std::shared_ptr<AWSSecurityGroup> sg : securityGroups) {
        this->table->setRowCount(row + 1);

        QString sgLabel = QString("SG: %1 (%2)").arg(sg->name).arg(sg->id);
        QTableWidgetItem *sgCaptionItem = new QTableWidgetItem(sgLabel);
        sgCaptionItem->setFlags(Qt::ItemIsEnabled);
        QFont font = sgCaptionItem->font();
        font.setBold(true);
        font.setPointSize(font.pointSize() + 2);
        sgCaptionItem->setFont(font);
        sgCaptionItem->setToolTip(sg->description);
        sgCaptionItem->setBackground(QBrush(QColor(245, 245, 245)));
        this->table->setItem(row, 0, sgCaptionItem);
        this->table->setSpan(row, 0, 1, 4);

        row++;
        this->table->setRowCount(row + 1);

        if (sg->ingressPermissions.count() > 0) {
            QTableWidgetItem *inboundItem = new QTableWidgetItem("Inbound");
            inboundItem->setFlags(Qt::ItemIsEnabled);
            font = inboundItem->font();
            font.setBold(true);
            inboundItem->setFont(font);
            inboundItem->setBackground(QBrush(QColor(248, 248, 248)));
            this->table->setItem(row, 0, inboundItem);
            this->table->setSpan(row, 0, 1, 4);

            row++;
            this->table->setRowCount(row + 1);
        }

        for (AWSIngressPermission perm : sg->ingressPermissions) {
            int i = 0;
            QString cidrs;
            for (QString cidr : perm.cidrs) {
                if (i != 0) {
                    cidrs += ",";
                }
                cidrs += cidr;
                i++;
            }

            QString ipProtocol = perm.ipProtocol;
            if (ipProtocol == "-1") {
                ipProtocol = "ALL";
            }

            this->table->setItem(row, 0, new QTableWidgetItem(ipProtocol));
            this->table->setItem(row, 1, new QTableWidgetItem(perm.fromPort));
            this->table->setItem(row, 2, new QTableWidgetItem(perm.toPort));
            this->table->setItem(row, 3, new QTableWidgetItem(cidrs));

            row++;
            this->table->setRowCount(row + 1);
        }

        if (sg->egressPermissions.count() > 0) {
            QTableWidgetItem *outboundItem = new QTableWidgetItem("Outbound");
            outboundItem->setFlags(Qt::ItemIsEnabled);
            font = outboundItem->font();
            font.setBold(true);
            outboundItem->setFont(font);
            outboundItem->setBackground(QBrush(QColor(248, 248, 248)));
            this->table->setItem(row, 0, outboundItem);
            this->table->setSpan(row, 0, 1, 4);

            row++;
            this->table->setRowCount(row + 1);
        }

        for (AWSEgressPermission perm : sg->egressPermissions) {
            int i = 0;
            QString cidrs;
            for (QString cidr : perm.cidrs) {
                if (i != 0) {
                    cidrs += ",";
                }
                cidrs += cidr;
                i++;
            }

            QString ipProtocol = perm.ipProtocol;
            if (ipProtocol == "-1") {
                ipProtocol = "ALL";
            }

            this->table->setItem(row, 0, new QTableWidgetItem(ipProtocol));
            this->table->setItem(row, 1, new QTableWidgetItem(perm.fromPort));
            this->table->setItem(row, 2, new QTableWidgetItem(perm.toPort));
            this->table->setItem(row, 3, new QTableWidgetItem(cidrs));

            row++;
            this->table->setRowCount(row + 1);
        }
    }

    this->table->setRowCount(row);
}
