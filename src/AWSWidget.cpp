#include "AWSWidget.h"

AWSWidget::AWSWidget()
{
    this->region = AWSConnector::LOCATION_US_EAST_1;
    this->firstTryToLogin = false;
    this->requestRunning = false;
    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

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
    for (int i = 0; i < this->instanceTable->horizontalHeader()->count(); i++) {
        this->instanceTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    QObject::connect(this->instanceTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectionChanged(QItemSelection, QItemSelection)));

    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    toolBarLayout->addWidget(this->toolBar);
    toolBarLayout->addWidget(this->regionComboBox);
    ((QVBoxLayout*) this->mainWidget->layout())->addLayout(toolBarLayout);
    this->mainWidget->layout()->addWidget(this->instanceTable);

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

AWSWidget::~AWSWidget()
{
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
    AWSInstance *instance;

    QModelIndexList indexes = this->instanceTable->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }

    instance = this->instanceModel->getInstance(indexes.first());
    std::cout << "test: " << instance->publicIP.toStdString() << std::endl;
    emit newConnection(*instance);
}

QVector<AWSInstance*> AWSWidget::parseDescribeInstancesResult(AWSResult *result)
{
    QVector<AWSInstance*> vector;
    AWSInstance *instance = NULL;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool instancesSet = false;
    bool instanceState = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "instancesSet") {
                instancesSet = true;
            } else if (name == "item") {
                itemLevel++;

                if (instancesSet && itemLevel == 2) {
                    instance = new AWSInstance();
                    instance->region = this->region;
                    vector << instance;
                }
            } else if (name == "instanceState") {
                instanceState = true;
            } else if (instancesSet && itemLevel == 2) {
                if (name == "instanceId") {
                    instance->id = xml.readElementText();
                } else if (name == "name" && instanceState == true) {
                    instance->status = xml.readElementText();
                } else if (name == "instanceType") {
                    instance->type = xml.readElementText();
                } else if (name == "keyName") {
                    instance->keyname = xml.readElementText();
                } else if (name == "ipAddress") {
                    instance->publicIP = xml.readElementText();
                } else if (name == "privateIpAddress") {
                    instance->privateIP = xml.readElementText();
                } else if (name == "launchTime") {
                    instance->launchTime = xml.readElementText();
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "instancesSet") {
                instancesSet = false;
            } else if (name == "item") {
                itemLevel--;
            } else if (name == "instanceState") {
                instanceState = false;
            }

            if (instancesSet && itemLevel < 2) {
                instance = NULL;
            }
        }
    }

    if (xml.hasError()) {
        std::cout << "XML error: " << xml.errorString().data() << std::endl;
    }

    return vector;
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

        QVector<AWSInstance*> vector = this->parseDescribeInstancesResult(result);
        this->instanceModel->setInstances(vector);
        this->connectButton->setEnabled(false);
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
