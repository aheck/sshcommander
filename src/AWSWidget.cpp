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
    this->toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_BrowserReload), "Refresh", this, SLOT(loadInstances()));
    this->connectButton = this->toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_CommandLink),
            "Connect to Instance", this, SLOT(connectToInstance()));
    this->connectButton->setEnabled(false);
    this->toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->searchLineEdit = new QLineEdit();
    this->searchLineEdit->setPlaceholderText(tr("Search"));
    QObject::connect(this->searchLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(searchForText(QString)));
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
    QObject::connect(this->instanceTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
    this->instanceTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->instanceTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showInstanceContextMenu(QPoint)));

    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    toolBarLayout->addWidget(this->toolBar);
    toolBarLayout->addWidget(this->searchLineEdit);
    toolBarLayout->addWidget(this->regionComboBox);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(toolBarLayout);
    this->mainWidget->layout()->addWidget(this->instanceTable);
    this->instanceNumLabel = new QLabel();
    this->mainWidget->layout()->addWidget(this->instanceNumLabel);

    this->setLayout(new QVBoxLayout(this));
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

    std::cout << "Trying to connect to AWS..." << std::endl;

    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->preferences->save();

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
    emit newConnection(instance);
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

            this->securityGroupsDialog->updateData(securityGroups);
        }
    }

    this->requestRunning = false;
    delete result;
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
        menu.addAction(tr("View Tags"), this, SLOT(showTags()));
        menu.addSeparator();
        QAction *connectToInstance = menu.addAction(tr("Connect to Instance"), this, SLOT(connectToInstance()));
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
