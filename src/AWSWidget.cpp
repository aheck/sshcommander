#include "AWSWidget.h"

#include "globals.h"

AWSWidget::AWSWidget()
{
    this->updatingVpcs = false;

    this->region = AWSConnector::LOCATION_US_EAST_1;
    this->firstTryToLogin = false;
    this->requestRunning = false;
    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, &AWSConnector::awsReplyReceived, this, &AWSWidget::handleAWSResult);

    this->securityGroupsDialog = new SecurityGroupsDialog(this);
    this->tagsDialog = new TagsDialog(this);
    this->vpcDialog = new VpcDialog(this);

    // build the loginWidget
    this->loginWidget = new QWidget();
    this->accessKeyLineEdit = new QLineEdit(this->loginWidget);
    this->secretKeyLineEdit = new QLineEdit(this->loginWidget);
    this->awsLoginButton = new QPushButton(tr("Login"), this->loginWidget);
    QVBoxLayout *loginLayout = new QVBoxLayout();
    loginLayout->setAlignment(Qt::AlignTop);
    QLabel *awsLoginLabel = new QLabel("Login to AWS");
    QFont font = awsLoginLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    awsLoginLabel->setFont(font);
    loginLayout->addWidget(awsLoginLabel);
    QFormLayout *awsFormLayout = new QFormLayout;
#ifdef Q_OS_MACOS
    awsFormLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
#endif
    awsFormLayout->addRow(tr("AWS Access Key:"), accessKeyLineEdit);
    awsFormLayout->addRow(tr("AWS Secret Key:"), secretKeyLineEdit);
    awsFormLayout->addRow("", this->awsLoginButton);

    loginLayout->addLayout(awsFormLayout);
    this->loginWidget->setLayout(loginLayout);

    QObject::connect(this->awsLoginButton, &QPushButton::clicked, this, &AWSWidget::connectToAWS);

    // build the mainWidget
    this->mainWidget = new QWidget();
    this->mainWidget->setLayout(new QVBoxLayout(this->mainWidget));
    this->toolBar = new QToolBar("toolBar", this->mainWidget);

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Refresh", this, &AWSWidget::loadData);
    this->connectButton = this->toolBar->addAction(QIcon(":/images/applications-internet.svg"),
            "Connect to Instance", this, &AWSWidget::connectToPublicIP);
    this->connectButton->setEnabled(false);
    this->toggleWindowButton = this->toolBar->addAction(QIcon(":/images/window-new.svg"),
            "Detach Window");
    connect(this->toggleWindowButton, &QAction::toggled, this, &AWSWidget::toggleWindowMode);
    this->toggleWindowButton->setCheckable(true);
    this->toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    this->vpcComboBox = new QComboBox(this);
    this->vpcComboBox->setMinimumWidth(250);
    this->vpcComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->vpcComboBox->setToolTip(tr("Virtual Private Cloud (VPC)"));
    // connect with old syntax because QComboBox::currentIndexChanged is overloaded
    QObject::connect(this->vpcComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVpc(int)));
    this->vpcToolBar = new QToolBar("vpcToolBar", this->mainWidget);
    this->showVpcButton = this->vpcToolBar->addAction(QIcon(":/images/dialog-information.svg"), "Show VPC Details", this, &AWSWidget::showVpcDialog);
    this->showVpcButton->setEnabled(false);
    this->vpcToolBar->addAction(QIcon(":/images/edit-clear.svg"), "Clear VPC", this, &AWSWidget::clearVpcComboBox);

    this->searchLineEdit = new QLineEdit(this);
    this->searchLineEdit->setPlaceholderText(tr("Filter by name, tag, instance ID, SSH key, IP or stack"));
    this->searchLineEdit->setClearButtonEnabled(true);
    QObject::connect(this->searchLineEdit, &QLineEdit::textEdited, this, &AWSWidget::searchForText);
    this->regionComboBox = new QComboBox();
    this->regionComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->regionComboBox->setToolTip(tr("Region"));

    for (int i = 0; i < AWSConnector::Regions.count(); i++) {
        this->regionComboBox->addItem(AWSConnector::Regions.at(i) + ": " + AWSConnector::RegionNames.at(i),
                AWSConnector::Regions.at(i));
    }

    QObject::connect(this->regionComboBox, &QComboBox::currentTextChanged, this, &AWSWidget::changeRegion);

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
    QObject::connect(this->instanceTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AWSWidget::selectionChanged);
    this->instanceTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->instanceTable, &QTableView::customContextMenuRequested, this, &AWSWidget::showInstanceContextMenu);
    connect(this->instanceTable, &QTableView::doubleClicked, this, &AWSWidget::doubleClicked);

    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    toolBarLayout->addWidget(this->toolBar);
    toolBarLayout->addWidget(this->searchLineEdit);
    toolBarLayout->addWidget(this->regionComboBox);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(toolBarLayout);

    // build the VPC "toolbar" layout
    QHBoxLayout *vpcLayout = new QHBoxLayout();
    QLabel *vpcLabel = new QLabel(tr("VPC: "));
    vpcLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vpcLayout->addWidget(vpcLabel);
    vpcLayout->addWidget(this->vpcComboBox);
    vpcLayout->addWidget(this->vpcToolBar);
    vpcLayout->addStretch(1);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(vpcLayout);

    this->mainWidget->layout()->addWidget(this->instanceTable);
    this->instanceNumLabel = new QLabel();
    this->mainWidget->layout()->addWidget(this->instanceNumLabel);
    this->mainWidget->layout()->setContentsMargins(0, 0, 0, 0);

    this->setLayout(new QVBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->loginWidget);
    this->layout()->addWidget(this->mainWidget);

    Preferences &preferences = Preferences::getInstance();

    // we only show the login widget if we haven't read any AWS credentials
    // from the program settings
    if (!preferences.getAWSAccessKey().isEmpty() && !preferences.getAWSSecretKey().isEmpty()) {
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
    Preferences &preferences = Preferences::getInstance();
    this->firstTryToLogin = true;

    preferences.setAWSAccessKey(this->accessKeyLineEdit->text());
    preferences.setAWSSecretKey(this->secretKeyLineEdit->text());

    this->loadData();
}

void AWSWidget::loadData()
{
    Preferences &preferences = Preferences::getInstance();

    if (this->requestRunning || preferences.getAWSAccessKey().isEmpty()) {
        return;
    }

    this->requestRunning = true;

    this->instanceModel->setVpcFilter(this->selectedVpcId);
    this->instanceModel->clear();

    qDebug() << "Trying to connect to AWS...";

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    preferences.save();

    this->awsConnector->setRegion(this->region);

    this->awsConnector->describeInstances();
    this->awsConnector->describeVpcs();
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
    qDebug() << "AWS Result received in AWSWidget";
    qDebug() << "Success: " << result->isSuccess;
    qDebug() << "HTTP Status: " << result->httpStatus;
    qDebug() << "Body: " << result->httpBody;

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
            std::vector<std::shared_ptr<AWSInstance>> instances = ::parseDescribeInstancesResponse(result, this->region);
            this->instanceModel->setInstances(instances);
            this->connectButton->setEnabled(false);
            this->updateNumberOfInstances();

            AWSCache &cache = AWSCache::getInstance();
            cache.clearInstances(this->region);
            cache.updateInstances(this->region, instances);

            this->instanceModel->resolveAllReferences();

            emit awsInstancesUpdated();
        } else if (result->responseType == "DescribeSecurityGroupsResponse") {
            std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups = parseDescribeSecurityGroupsResponse(result, this->region);

            this->securityGroupsDialog->updateData(securityGroups);

            AWSCache &cache = AWSCache::getInstance();
            cache.updateSecurityGroups(this->region, securityGroups);
        } else if (result->responseType == "DescribeVpcsResponse") {
            std::vector<std::shared_ptr<AWSVpc>> vpcs = parseDescribeVpcsResponse(result, this->region);

            if (result->userType == "VpcDialog") {
                this->vpcDialog->updateData(vpcs);

                AWSCache &cache = AWSCache::getInstance();
                cache.updateVpcs(this->region, vpcs);
            } else {
                this->updateVpcs(vpcs);

                AWSCache &cache = AWSCache::getInstance();
                cache.clearVpcs(this->region);
                cache.updateVpcs(this->region, vpcs);
            }

            this->instanceModel->resolveAllReferences();
        }
    }

    this->requestRunning = false;
    delete result;
}

void AWSWidget::changeRegion(QString regionText)
{
    if (this->region == regionText) {
        return;
    }

    // if we switch regions the VPC ID becomes invalid
    this->selectedVpcId = "";

    this->region = this->regionComboBox->itemData(this->regionComboBox->findText(regionText)).toString();
    this->loadData();
}

void AWSWidget::changeVpc(int index)
{
    if (this->updatingVpcs) {
        return;
    }

    this->selectedVpcId = this->vpcComboBox->currentData(Qt::UserRole).toString();
    this->updateShowVpcButton();
    this->instanceModel->setVpcFilter(this->selectedVpcId);
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
    QModelIndex index = this->instanceTable->indexAt(pos);

    if (index.isValid()) {
        QMenu menu;

        this->clipboardCandidate = this->instanceModel->data(index, Qt::DisplayRole).toString();
        if (!this->clipboardCandidate.isEmpty()) {
            menu.addAction("Copy '" + this->clipboardCandidate + "' to Clipboard", this, &AWSWidget::copyItemToClipboard);
            menu.addSeparator();
        }

        std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

        QMenu *instanceActions = menu.addMenu(tr("Instance Actions"));

        QAction *startAction = instanceActions->addAction(tr("Start"), this, &AWSWidget::startInstance);
        if (instance->status != "stopped") {
            startAction->setEnabled(false);
        }

        QAction *stopAction = instanceActions->addAction(tr("Stop"), this, &AWSWidget::stopInstance);
        if (instance->status != "running") {
            stopAction->setEnabled(false);
        }

        QAction *rebootAction = instanceActions->addAction(tr("Reboot"), this, &AWSWidget::rebootInstance);
        if (instance->status != "running") {
            rebootAction->setEnabled(false);
        }

        QAction *terminateAction = instanceActions->addAction(tr("Terminate"), this, &AWSWidget::terminateInstance);
        if (instance->status != "pending" && instance->status != "running") {
            terminateAction->setEnabled(false);
        }

        menu.addAction(tr("View Security Groups"), this, &AWSWidget::showSecurityGroups);
        menu.addAction(tr("View Tags"), this, &AWSWidget::showTags);

        menu.addSeparator();

        this->vpcIdCandidate = instance->vpcId;
        menu.addAction(tr("Filter by Instance VPC"), this, &AWSWidget::selectVpc);

        menu.addSeparator();

        menu.addAction(tr("Connect to Private IP"), this, &AWSWidget::connectToPrivateIP);
        QAction *connectToInstance = menu.addAction(tr("Connect to Instance"), this, &AWSWidget::connectToPublicIP);
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

void AWSWidget::showVpcDialog()
{
    if (this->vpcComboBox->currentIndex() == 0) {
        return;
    }

    this->vpcDialog->showDialog(this->awsConnector, this->vpcComboBox->currentData().toString(), "");
}

void AWSWidget::searchForText(const QString &text)
{
    this->instanceModel->setSearchTextFilter(text);
}

void AWSWidget::copyItemToClipboard()
{
    QGuiApplication::clipboard()->setText(this->clipboardCandidate);
}

void AWSWidget::updateVpcs(std::vector<std::shared_ptr<AWSVpc>> &vpcs)
{
    this->updatingVpcs = true;
    this->vpcComboBox->clear();

    this->vpcComboBox->addItem("All VPCs", QVariant(""));
    QString label;

    for (auto vpc : vpcs) {
        if (vpc->name.isEmpty()) {
            label = vpc->id;
        } else {
            label = vpc->name + " (" + vpc->id + ")";
        }

        if (vpc->isDefault) {
            label += " [default]";
        }

        this->vpcComboBox->addItem(label, QVariant(vpc->id));
    }

    this->updatingVpcs = false;

    if (!this->selectedVpcId.isEmpty()) {
        int currentIndex = this->vpcComboBox->findData(QVariant(this->selectedVpcId));
        qDebug() << "updateVpcs: currentIndex: " << currentIndex;

        // reset selected VPC ID in case the VPC no longer exists
        if (currentIndex == -1) {
            this->selectedVpcId = "";
            this->instanceModel->setVpcFilter(this->selectedVpcId);
        } else {
            // select the right VPC in the combo box
            this->vpcComboBox->setCurrentIndex(currentIndex);
        }
    }

    this->updateShowVpcButton();
}

void AWSWidget::clearVpcComboBox()
{
    this->vpcComboBox->setCurrentIndex(0);
    this->showVpcButton->setEnabled(false);
}

void AWSWidget::updateShowVpcButton() {
    this->showVpcButton->setEnabled(this->vpcComboBox->currentIndex() != 0);
}

void AWSWidget::selectVpc()
{
    int index = this->vpcComboBox->findData(this->vpcIdCandidate);

    if (index > -1) {
        this->vpcComboBox->setCurrentIndex(index);
        this->updateShowVpcButton();
    }
}

void AWSWidget::stopInstance()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    auto reply = QMessageBox::question(this, tr("Stopping Instance"),
            QString("Do you really want to stop the instance '%1'?").arg(instance->formattedName()),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->region);

    this->awsConnector->stopInstances(instanceIds);
}

void AWSWidget::startInstance()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->region);

    this->awsConnector->startInstances(instanceIds);
}

void AWSWidget::rebootInstance()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    auto reply = QMessageBox::question(this, tr("Rebooting Instance"),
            QString("Do you really want to reboot the instance '%1'?").arg(instance->formattedName()),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->region);

    this->awsConnector->rebootInstances(instanceIds);
}

void AWSWidget::terminateInstance()
{
    std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();

    auto reply = QMessageBox::question(this, tr("Terminating Instance"),
            QString("Do you really want to terminate the instance '%1'?").arg(instance->formattedName()),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->region);

    this->awsConnector->terminateInstances(instanceIds);
}

void AWSWidget::toggleWindowMode(bool checked)
{
    if (checked) {
        this->toggleWindowButton->setToolTip(tr("Reattach Window"));
    } else {
        this->toggleWindowButton->setToolTip(tr("Detach Window"));
    }

    emit requestToggleDetach(checked);
}

void AWSWidget::reattach()
{
    this->toggleWindowButton->setChecked(false);
}
