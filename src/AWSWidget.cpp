#include "AWSWidget.h"

AWSWidget::AWSWidget()
{
    this->region = AWSConnector::LOCATION_US_EAST_1;
    this->firstTryToLogin = false;
    this->requestRunning = false;
    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->securityGroupsDialog = new SecurityGroupsDialog();

    // build the loginWidget
    this->loginWidget = new QWidget();
    this->accessKeyLineEdit = new QLineEdit(this->loginWidget);
    this->secretKeyLineEdit = new QLineEdit(this->loginWidget);
    this->awsLoginButton = new QPushButton(tr("Login"), this->loginWidget);
    QFormLayout *awsFormLayout = new QFormLayout;
    awsFormLayout->addRow(tr("AWS Access Key:"), accessKeyLineEdit);
    awsFormLayout->addRow(tr("AWS Secret Key:"), secretKeyLineEdit);
    awsFormLayout->addRow("", this->awsLoginButton);
    this->loginWidget->setLayout(awsFormLayout);
    QObject::connect(this->awsLoginButton, SIGNAL(clicked()), this, SLOT(connectToAWS()));

    // build the mainWidget
    this->mainWidget = new QWidget();
    this->mainWidget->setLayout(new QVBoxLayout(this->mainWidget));
    this->toolBar = new QToolBar("toolBar", this->mainWidget);
    this->toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_BrowserReload), "Refresh", this, SLOT(loadInstances()));
    this->connectButton = this->toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_CommandLink), "Connect to Instance", this, SLOT(connectToInstance()));
    this->connectButton->setEnabled(false);
    this->toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->regionComboBox = new QComboBox();
    this->regionComboBox->addItems(AWSConnector::Regions);
    this->regionComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    QObject::connect(this->regionComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(changeRegion(QString)));
    this->instanceTable = new QTableView(this->mainWidget);
    this->instanceModel = new InstanceItemModel();
    this->instanceTable->setModel(this->instanceModel);
    this->instanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->instanceTable->setSortingEnabled(true);
    this->instanceTable->horizontalHeader()->setStretchLastSection(true);
    for (int i = 0; i < this->instanceTable->horizontalHeader()->count(); i++) {
        this->instanceTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    QObject::connect(this->instanceTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
    this->instanceTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->instanceTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showInstanceContextMenu(QPoint)));

    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    toolBarLayout->addWidget(this->toolBar);
    toolBarLayout->addWidget(this->regionComboBox);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(toolBarLayout);
    this->mainWidget->layout()->addWidget(this->instanceTable);
    this->instanceNumLabel = new QLabel();
    this->mainWidget->layout()->addWidget(this->instanceNumLabel);

    this->setLayout(new QVBoxLayout(this));
    this->layout()->addWidget(this->loginWidget);
    this->layout()->addWidget(this->mainWidget);

    this->readSettings();

    // we only show the login widget if we haven't read any AWS credentials
    // from the program settings
    if (!this->accessKey.isEmpty() && !this->secretKey.isEmpty()) {
        this->loginWidget->setVisible(false);
        this->curWidget = this->mainWidget;
    } else {
        this->mainWidget->setVisible(false);
        this->curWidget = this->loginWidget;
    }
}

void AWSWidget::connectToAWS()
{
    this->firstTryToLogin = true;

    this->accessKey = this->accessKeyLineEdit->text();
    this->secretKey = this->secretKeyLineEdit->text();

    this->loadInstances();
}

void AWSWidget::loadInstances()
{
    if (this->requestRunning || this->accessKey.isEmpty()) {
        return;
    }

    this->requestRunning = true;

    std::cout << "Trying to connect to AWS..." << std::endl;

    this->awsConnector->setAccessKey(this->accessKey);
    this->awsConnector->setSecretKey(this->secretKey);
    this->awsConnector->setRegion(this->region);

    this->awsConnector->describeInstances();
}

void AWSWidget::connectToInstance()
{
    std::shared_ptr<AWSInstance> instance;

    QModelIndexList indexes = this->instanceTable->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }

    instance = this->instanceModel->getInstance(indexes.first());
    std::cout << "test: " << instance->publicIP.toStdString() << std::endl;
    emit newConnection(*instance);
}

void AWSWidget::updateNumberOfInstances()
{
    int numberOfInstances = this->instanceModel->rowCount(QModelIndex());

    if (numberOfInstances == 1) {
        this->instanceNumLabel->setText("1 Instance");
    } else {
        this->instanceNumLabel->setText(QString("%1 Instances").arg(numberOfInstances));
    }
}

void AWSWidget::handleAWSResult(AWSResult *result)
{
    std::cout << "AWS Result received in AWSWidget" << std::endl;
    std::cout << "Success: " << result->isSuccess << std::endl;
    std::cout << "HTTP Status: " << result->httpStatus << std::endl;
    std::cout << "Body: " << result->httpBody.toStdString() << std::endl;

    if (!result->isSuccess) {
        QMessageBox msgBox;
        msgBox.setText(QString("AWS communication error: ") + result->errorString);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    } else {
        if (this->curWidget == this->loginWidget) {
            this->loginWidget->setVisible(false);
            this->mainWidget->setVisible(true);
            this->curWidget = this->mainWidget;
        }

        if (result->responseType == "DescribeInstancesResponse") {
            QVector<std::shared_ptr<AWSInstance>> vector = ::parseDescribeInstancesResponse(result, this->region);
            this->instanceModel->setInstances(vector);
            this->connectButton->setEnabled(false);
            this->updateNumberOfInstances();
        } else if (result->responseType == "DescribeSecurityGroupsResponse") {
            QVector<std::shared_ptr<AWSSecurityGroup>> securityGroups = parseDescribeSecurityGroupsResponse(result, this->region);

            for (std::shared_ptr<AWSSecurityGroup> sg : securityGroups) {
                QListWidgetItem *item = new QListWidgetItem(QString("SG: %1 (%2)").arg(sg->name).arg(sg->id),
                        this->securityGroupsDialog->list, QListWidgetItem::Type);
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                item->setToolTip(sg->description);
                this->securityGroupsDialog->list->addItem(item);
                this->securityGroupsDialog->list->addItem("Ingress");
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

                    this->securityGroupsDialog->list->addItem(item);
                }

                this->securityGroupsDialog->list->addItem("Egress");
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

                    this->securityGroupsDialog->list->addItem(item);
                }
            }
        }
    }

    this->requestRunning = false;
    delete result;
}

void AWSWidget::readSettings()
{
    QSettings settings;

    settings.beginGroup("AWS");
    this->accessKey = settings.value("accessKey", "").toString();
    this->secretKey = settings.value("secretKey", "").toString();
    settings.endGroup();
}

void AWSWidget::saveAWSCredentials()
{
    QSettings settings;

    settings.beginGroup("AWS");
    settings.setValue("accessKey", this->accessKey);
    settings.setValue("secretKey", this->secretKey);
    settings.endGroup();
}

void AWSWidget::changeRegion(QString region)
{
    this->region = region;
    this->loadInstances();
}

void AWSWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    bool enabled = selected.size() != 0;

    if (enabled) {
        QModelIndex index = selected.indexes().first();
        std::shared_ptr<AWSInstance> instance = this->instanceModel->getInstance(index);
        if (instance->publicIP.isEmpty() || instance->status != "running") {
            enabled = false;
        }
    }

    this->connectButton->setEnabled(enabled);
}

QString AWSWidget::getRegion() const
{
    return this->region;
}

void AWSWidget::setRegion(const QString region)
{
    this->region = region;
    this->regionComboBox->setCurrentIndex(this->regionComboBox->findText(region));
}

void AWSWidget::showInstanceContextMenu(QPoint pos)
{
    QPoint globalPos = this->instanceTable->mapToGlobal(pos);

    if (this->instanceTable->indexAt(pos).isValid()) {
        QMenu menu;
        menu.addAction(tr("View Security Groups"), this, SLOT(showSecurityGroups()));

        menu.exec(globalPos);
    }
}

void AWSWidget::showSecurityGroups()
{
    QString title;
    QModelIndexList indexes = this->instanceTable->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) {
        return;
    }

    std::shared_ptr<AWSInstance> instance = this->instanceModel->getInstance(indexes.first());

    if (instance == nullptr) {
        return;
    }

    if (instance->name.isEmpty()) {
        title = QString("Security Groups of instance %1").arg(instance->id);
    } else {
        title = QString("Security Groups of instance '%1' (%2)").arg(instance->name).arg(instance->id);
    }

    this->securityGroupsDialog->setWindowTitle(title);

    this->securityGroupsDialog->list->clear();

    QList<QString> groupIds;
    for (AWSSecurityGroup sg : instance->securityGroups) {
        groupIds.append(sg.id);
    }
    this->awsConnector->describeSecurityGroups(groupIds);

    this->securityGroupsDialog->exec();
}
