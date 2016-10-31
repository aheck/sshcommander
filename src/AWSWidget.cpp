#include "AWSWidget.h"

AWSWidget::AWSWidget()
{
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
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "Refresh", this, SLOT(connectToAWS()));
    this->instanceTable = new QTableWidget(this->mainWidget);
    this->instanceTable->setColumnCount(6);
    QStringList headerLabels;
    headerLabels << QString("ID");
    headerLabels << QString("Status");
    headerLabels << QString("Type");
    headerLabels << QString("Public IP");
    headerLabels << QString("Private IP");
    headerLabels << QString("Launch Time");
    this->instanceTable->setHorizontalHeaderLabels(headerLabels);
    this->mainWidget->layout()->addWidget(this->toolBar);
    this->mainWidget->layout()->addWidget(this->instanceTable);
    this->mainWidget->setVisible(false);

    this->setLayout(new QVBoxLayout(this));
    this->layout()->addWidget(this->loginWidget);
    this->layout()->addWidget(this->mainWidget);
    this->curWidget = this->loginWidget;
}

AWSWidget::~AWSWidget()
{
}

void AWSWidget::connectToAWS()
{
    if (this->requestRunning) {
        return;
    }

    this->requestRunning = true;

    QString accessKey = this->accessKeyLineEdit->text();
    QString secretKey = this->secretKeyLineEdit->text();

    std::cout << "Trying to connect to AWS..." << std::endl;

    this->awsConnector->setAccessKey(accessKey);
    this->awsConnector->setSecretKey(secretKey);
    this->awsConnector->setRegion(AWSConnector::LOCATION_US_EAST_1);

    this->awsConnector->describeInstances();
}

QVector<AWSInstance*>* AWSWidget::parseDescribeInstancesResult(AWSResult *result)
{
    QVector<AWSInstance*> *vector = new QVector<AWSInstance*>;

    AWSInstance *instance = new AWSInstance();
    instance->id = "i-123456";
    instance->status = "terminated";
    instance->type = "m8.supersized";
    instance->publicIP = "1.2.3.4";
    instance->privateIP = "192.168.1.1";
    instance->launchTime = "yesterday";

    vector->append(instance);

    return vector;
}

void AWSWidget::handleAWSResult(AWSResult *result)
{
    std::cout << "AWS Result received in MainWindow" << std::endl;
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

        QVector<AWSInstance*>* vector = this->parseDescribeInstancesResult(result);

        for (QVector<AWSInstance*>::iterator it = vector->begin(); it != vector->end(); it++) {
            int curRow = this->instanceTable->rowCount();
            this->instanceTable->insertRow(curRow);
            this->instanceTable->setItem(curRow, 0, new QTableWidgetItem((*it)->id, 0));
            this->instanceTable->setItem(curRow, 1, new QTableWidgetItem((*it)->status, 0));
            this->instanceTable->setItem(curRow, 2, new QTableWidgetItem((*it)->type, 0));
            this->instanceTable->setItem(curRow, 3, new QTableWidgetItem((*it)->publicIP, 0));
            this->instanceTable->setItem(curRow, 4, new QTableWidgetItem((*it)->privateIP, 0));
            this->instanceTable->setItem(curRow, 5, new QTableWidgetItem((*it)->launchTime, 0));
            curRow++;
        }
    }

    this->requestRunning = false;
    delete result;
}
