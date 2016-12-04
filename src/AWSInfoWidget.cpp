#include "AWSInfoWidget.h"

AWSInfoWidget::AWSInfoWidget(Preferences *preferences)
{
    this->preferences = preferences;

    this->enabled = false;

    this->securityGroupsDialog = new SecurityGroupsDialog();

    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->awsPage = new QWidget();
    this->awsContent = new QWidget();
    this->noAWSPage = new QWidget();
    QVBoxLayout *noAWSLayout = new QVBoxLayout();
    noAWSLayout->setAlignment(Qt::AlignCenter);
    QLabel *noAWSLabel = new QLabel("No AWS Data");
    QFont font = noAWSLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    noAWSLabel->setFont(font);
    noAWSLabel->setStyleSheet("QLabel { color : grey; }");
    noAWSLayout->addWidget(noAWSLabel);
    this->noAWSPage->setLayout(noAWSLayout);

    this->toolBar = new QToolBar();
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_BrowserReload), "Reload", this, SLOT(reloadInstanceData()));
    this->awsPage->setLayout(new QVBoxLayout());
    this->awsPage->layout()->addWidget(this->toolBar);

    this->scrollArea = new QScrollArea();
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setWidget(this->awsContent);
    this->awsPage->layout()->addWidget(this->scrollArea);

    this->widgetStack = new QStackedWidget();
    this->widgetStack->addWidget(this->noAWSPage);
    this->widgetStack->addWidget(this->awsPage);

    this->labelInstanceId = new QLabel("Instance ID:");
    this->labelName = new QLabel("Name:");
    this->labelRegion = new QLabel("Region:");
    this->labelAvailabilityZone = new QLabel("Availability Zone:");
    this->labelStatus = new QLabel("Status:");
    this->labelKeyname = new QLabel("Keyname:");
    this->labelType = new QLabel("Type:");
    this->labelImageId = new QLabel("Image ID:");
    this->labelLaunchTime = new QLabel("Launch Time:");
    this->labelPublicIP = new QLabel("Public IP:");
    this->labelPrivateIP = new QLabel("Private IP:");
    this->labelSubnetId = new QLabel("Subnet ID:");
    this->labelVpcId = new QLabel("VPC ID:");
    this->labelTags = new QLabel("Tags:");
    this->labelTags->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->labelSecurityGroups = new QLabel("Security Groups:");
    this->labelVirtualizationType = new QLabel("Virtualization Type:");
    this->labelArchitecture = new QLabel("Architecture:");
    this->labelHypervisor = new QLabel("Hypervisor:");

    this->valueInstanceId = new QLabel("");
    this->valueInstanceId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueName = new QLabel("");
    this->valueName->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueRegion = new QLabel("");
    this->valueRegion->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueAvailabilityZone = new QLabel("");
    this->valueAvailabilityZone->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueStatus = new QLabel("");
    this->valueStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueKeyname = new QLabel("");
    this->valueKeyname->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueType = new QLabel("");
    this->valueType->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueImageId = new QLabel("");
    this->valueImageId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueLaunchTime = new QLabel("");
    this->valueLaunchTime->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valuePublicIP = new QLabel("");
    this->valuePublicIP->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valuePrivateIP = new QLabel("");
    this->valuePrivateIP->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSubnetId = new QLabel("");
    this->valueSubnetId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueVpcId = new QLabel("");
    this->valueVpcId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueTags = new QLabel("");
    this->valueTags->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSecurityGroups = new QLabel();
    this->valueSecurityGroups->setText("<a href=\"http://localhost/\">View Security Groups</a>");
    this->valueSecurityGroups->setTextFormat(Qt::RichText);
    this->valueSecurityGroups->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QObject::connect(this->valueSecurityGroups, SIGNAL(linkActivated(QString)), this, SLOT(showSecurityGroups()));
    this->valueVirtualizationType = new QLabel("");
    this->valueVirtualizationType->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueArchitecture = new QLabel("");
    this->valueArchitecture->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueHypervisor = new QLabel("");
    this->valueHypervisor->setTextInteractionFlags(Qt::TextSelectableByMouse);

    this->gridLayout = new QGridLayout();
    this->gridLayout->addWidget(this->labelInstanceId, 0, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueInstanceId, 0, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelName, 1, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueName, 1, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelRegion, 2, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueRegion, 2, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelAvailabilityZone, 3, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueAvailabilityZone, 3, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelStatus, 4, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueStatus, 4, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelKeyname, 5, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueKeyname, 5, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelType, 6, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueType, 6, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelImageId, 7, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueImageId, 7, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelLaunchTime, 8, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueLaunchTime, 8, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelPublicIP, 9, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePublicIP, 9, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelPrivateIP, 10, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePrivateIP, 10, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelSubnetId, 11, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSubnetId, 11, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelVpcId, 12, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVpcId, 12, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelTags, 13, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueTags, 13, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSecurityGroups, 14, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSecurityGroups, 14, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelVirtualizationType, 15, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVirtualizationType, 15, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelArchitecture, 16, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueArchitecture, 16, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelHypervisor, 17, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHypervisor, 17, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(18, 1);
    this->gridLayout->setColumnStretch(2, 1);

    this->awsContent->setLayout(this->gridLayout);

    this->mainLayout = new QVBoxLayout();
    this->mainLayout->addWidget(this->widgetStack);
    this->setLayout(mainLayout);
}

AWSInfoWidget::~AWSInfoWidget()
{
    delete this->securityGroupsDialog;
    delete this->awsConnector;
}

void AWSInfoWidget::updateData(std::shared_ptr<AWSInstance> instance)
{
    this->instance = instance;

    this->valueInstanceId->setText(instance->id);
    this->valueName->setText(instance->name);
    this->valueRegion->setText(instance->region);
    this->valueAvailabilityZone->setText(instance->availabilityZone);

    this->valueStatus->setText(instance->status);
    if (instance->status == "running") {
        this->valueStatus->setStyleSheet("QLabel { background-color : green; color: white; }");
    } else if (instance->status == "terminated") {
        this->valueStatus->setStyleSheet("QLabel { background-color : red; color: white; }");
    } else {
        this->valueStatus->setStyleSheet("QLabel { background-color : yellow; color: white; }");
    }
    this->valueKeyname->setText(instance->keyname);
    this->valueType->setText(instance->type);
    this->valueImageId->setText(instance->imageId);

    this->valueLaunchTime->setText(instance->launchTime);
    this->valuePublicIP->setText(instance->publicIP);
    this->valuePrivateIP->setText(instance->privateIP);
    this->valueSubnetId->setText(instance->subnetId);

    this->valueVpcId->setText(instance->vpcId);
    this->valueVirtualizationType->setText(instance->virtualizationType);
    this->valueArchitecture->setText(instance->architecture);
    this->valueHypervisor->setText(instance->hypervisor);

    QString tagsValue;
    int i = 0;
    for (AWSTag tag : instance->tags) {
        if (i != 0) {
            tagsValue += "\n";
        }

        tagsValue += tag.key + " = " + tag.value;
        i++;
    }
    this->valueTags->setText(tagsValue);
}

void AWSInfoWidget::setAWSEnabled(bool enabled)
{
    this->enabled = enabled;

    if (this->enabled) {
        this->widgetStack->setCurrentIndex(1);
    } else {
        this->widgetStack->setCurrentIndex(0);
    }
}

void AWSInfoWidget::handleAWSResult(AWSResult *result)
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
        if (result->responseType == "DescribeSecurityGroupsResponse") {
            QVector<std::shared_ptr<AWSSecurityGroup>> securityGroups = parseDescribeSecurityGroupsResponse(result, this->instance->region);

            this->securityGroupsDialog->updateData(securityGroups);
        } else if (result->responseType == "DescribeInstancesResponse") {
            std::cout << "Describe Instances" << std::endl;

            QVector<std::shared_ptr<AWSInstance>> instances = parseDescribeInstancesResponse(result, this->instance->region);

            if (instances.count() == 1) {
                std::shared_ptr<AWSInstance> newInstance = instances.at(0);
                *(this->instance) = *newInstance;
                this->updateData(instance);
            }
        }
    }

    delete result;
}

void AWSInfoWidget::showSecurityGroups()
{
    std::cout << "Show security groups..." << std::endl;

    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);
    this->securityGroupsDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoWidget::reloadInstanceData()
{
    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    this->awsConnector->describeInstances(instanceIds);
}
