#include "AWSWidget.h"

AWSWidget::AWSWidget(Preferences *preferences)
{
    this->preferences = preferences;

    this->region = AWSConnector::LOCATION_US_EAST_1;
    this->firstTryToLogin = false;
    this->requestRunning = false;
    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->securityGroupsDialog = new SecurityGroupsDialog();
    this->tagsDialog = new TagsDialog();

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
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Refresh", this, SLOT(loadInstances()));
    this->connectButton = this->toolBar->addAction(QIcon(":/images/applications-internet.svg"),
            "Connect to Instance", this, SLOT(connectToPublicIP()));
    this->connectButton->setEnabled(false);
    this->toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->searchLineEdit = new QLineEdit();
    this->searchLineEdit->setPlaceholderText(tr("Filter by name, tag, instance ID, SSH key, IP or stack"));
    this->searchLineEdit->setClearButtonEnabled(true);
    QObject::connect(this->searchLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(searchForText(QString)));
    this->regionComboBox = new QComboBox();
    this->regionComboBox->setToolTip(tr("Region"));

    for (int i = 0; i < AWSConnector::Regions.count(); i++) {
        this->regionComboBox->addItem(AWSConnector::Regions.at(i) + ": " + AWSConnector::RegionNames.at(i),
                AWSConnector::Regions.at(i));
    }

    this->regionComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    QObject::connect(this->regionComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(changeRegion(QString)));
    this->instanceTable = new QTableView(this->mainWidget);
    this->instanceModel = new InstanceItemModel();
    this->instanceTable->setModel(this->instanceModel);
    this->instanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->instanceTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->instanceTable->setSortingEnabled(true);
    for (int i = 0; i < this->instanceTable->horizontalHeader()->count(); i++) {
        this->instanceTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    this->instanceTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    this->instanceTable->horizontalHeader()->setStretchLastSection(true);
    QObject::connect(this->instanceTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
    this->instanceTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->instanceTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showInstanceContextMenu(QPoint)));
    connect(this->instanceTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));

    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    toolBarLayout->addWidget(this->toolBar);
    toolBarLayout->addWidget(this->searchLineEdit);
    toolBarLayout->addWidget(this->regionComboBox);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(toolBarLayout);
    this->mainWidget->layout()->addWidget(this->instanceTable);
    this->instanceNumLabel = new QLabel();
    this->mainWidget->layout()->addWidget(this->instanceNumLabel);

    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->loginWidget);
    this->layout()->addWidget(this->mainWidget);

    // we only show the login widget if we haven't read any AWS credentials
    // from the program settings
    if (!this->preferences->getAWSAccessKey().isEmpty() && !this->preferences->getAWSSecretKey().isEmpty()) {
        this->loginWidget->setVisible(false);
        this->curWidget = this->mainWidget;
    } else {
        this->mainWidget->setVisible(false);
        this->curWidget = this->loginWidget;
    }
}

void AWSWidget::doubleClicked(const QModelIndex &index)
{
    if (this->connectButton->isEnabled()) {
        this->connectToPublicIP();
    } else {
        this->connectToPrivateIP();
    }
}

void AWSWidget::connectToAWS()
{
    this->firstTryToLogin = true;

    this->preferences->setAWSAccessKey(this->accessKeyLineEdit->text());
    this->preferences->setAWSSecretKey(this->secretKeyLineEdit->text());

    this->loadInstances();
}

void AWSWidget::loadInstances()
{
    if (this->requestRunning || this->preferences->getAWSAccessKey().isEmpty()) {
        return;
    }

    this->requestRunning = true;

    this->instanceModel->clear();

    std::cout << "Trying to connect to AWS..." << std::endl;

    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->preferences->save();

    this->awsConnector->setRegion(this->region);

    this->awsConnector->describeInstances();
}

void AWSWidget::connectToPublicIP()
{
    this->connectToInstance(false);
}

void AWSWidget::connectToPrivateIP()
{
    this->connectToInstance(true);
}

void AWSWidget::connectToInstance(bool toPrivateIP)
{
    std::shared_ptr<AWSInstance> instance;

    QModelIndexList indexes = this->instanceTable->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }

    instance = this->instanceModel->getInstance(indexes.first());

    if (instance->status != "running") {
        return;
    }

    InstanceItemModel *model = static_cast<InstanceItemModel *>(this->instanceTable->model());
    auto vpcNeighbours = model->getInstancesByVpcId(instance->vpcId);
    auto newEnd = std::remove_if(vpcNeighbours.begin(), vpcNeighbours.end(),
            [instance](const std::shared_ptr<AWSInstance> cur) {return cur->id == instance->id;});
    vpcNeighbours.erase(newEnd, vpcNeighbours.end());

    emit newConnection(instance, vpcNeighbours, toPrivateIP);
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
            std::vector<std::shared_ptr<AWSInstance>> vector = ::parseDescribeInstancesResponse(result, this->region);
            this->instanceModel->setInstances(vector);
            this->connectButton->setEnabled(false);
            this->updateNumberOfInstances();
        } else if (result->responseType == "DescribeSecurityGroupsResponse") {
            std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups = parseDescribeSecurityGroupsResponse(result, this->region);

            this->securityGroupsDialog->updateData(securityGroups);
        }
    }

    this->requestRunning = false;
    delete result;
}

void AWSWidget::changeRegion(QString regionText)
{
    this->region = this->regionComboBox->itemData(this->regionComboBox->findText(regionText)).toString();
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
    this->regionComboBox->setCurrentIndex(this->regionComboBox->findData(region));
}

void AWSWidget::showInstanceContextMenu(QPoint pos)
{
    QPoint globalPos = this->instanceTable->mapToGlobal(pos);

    if (this->instanceTable->indexAt(pos).isValid()) {
        QMenu menu;
        menu.addAction(tr("View Security Groups"), this, SLOT(showSecurityGroups()));
        menu.addAction(tr("View Tags"), this, SLOT(showTags()));

        menu.addSeparator();

        menu.addAction(tr("Connect to Private IP"), this, SLOT(connectToPrivateIP()));
        QAction *connectToInstance = menu.addAction(tr("Connect to Instance"), this, SLOT(connectToPublicIP()));
        connectToInstance->setIcon(QIcon(":/images/applications-internet.svg"));
        connectToInstance->setEnabled(this->connectButton->isEnabled());

        menu.exec(globalPos);
    }
}

std::shared_ptr<AWSInstance> AWSWidget::getSelectedInstance()
{
    QModelIndexList indexes = this->instanceTable->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) {
        return nullptr;
    }

    return this->instanceModel->getInstance(indexes.first());
}

void AWSWidget::showSecurityGroups()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    if (instance == nullptr) {
        return;
    }

    this->securityGroupsDialog->showDialog(this->awsConnector, instance);
}

void AWSWidget::showTags()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    if (instance == nullptr) {
        return;
    }

    this->tagsDialog->showDialog(instance);
}

void AWSWidget::searchForText(const QString &text)
{
    this->instanceModel->setSearchText(text);
}
