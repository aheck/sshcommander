#include "AWSInfoWidget.h"

AWSInfoWidget::AWSInfoWidget(Preferences *preferences)
{
    this->preferences = preferences;

    this->enabled = false;

    this->securityGroupsDialog = new SecurityGroupsDialog();
    this->imageDialog = new ImageDialog();
    this->subnetDialog = new SubnetDialog();
    this->vpcDialog = new VpcDialog();

    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->awsPage = new QWidget();
    this->awsContent = new QWidget();
    this->disabledWidget = new DisabledWidget("No AWS Data");

    this->toolBar = new QToolBar();
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadInstanceData()));
    this->toolBar->setOrientation(Qt::Vertical);
    this->awsPage->setLayout(new QHBoxLayout());
    this->awsPage->layout()->setContentsMargins(0, 0, 0, 0);
    this->awsPage->layout()->addWidget(this->toolBar);

    this->scrollArea = new QScrollArea();
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setWidget(this->awsContent);
    this->awsPage->layout()->addWidget(this->scrollArea);

    this->widgetStack = new QStackedWidget();
    this->widgetStack->layout()->setContentsMargins(0, 0, 0, 0);
    this->widgetStack->addWidget(this->disabledWidget); this->widgetStack->addWidget(this->awsPage);

    this->labelInstanceId = new QLabel("Instance ID:");
    this->labelName = new QLabel("Name:");
    this->labelRegion = new QLabel("Region:");
    this->labelAvailabilityZone = new QLabel("Availability Zone:");
    this->labelStatus = new QLabel("Status:");
    this->labelKeyname = new QLabel("Keyname:");
    this->labelType = new QLabel("Type:");
    this->labelImage = new QLabel("Image (AMI):");
    this->labelLaunchTime = new QLabel("Launch Time:");
    this->labelPublicIP = new QLabel("Public IP:");
    this->labelPrivateIP = new QLabel("Private IP:");
    this->labelVpc = new QLabel("VPC ID:");
    this->labelSubnet = new QLabel("Subnet:");
    this->labelSourceDestCheck = new QLabel("Source Dest Check:");
    this->labelCfStackName = new QLabel("CloudFormation Stack:");
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
    this->valueImage = new QLabel("");
    this->valueImage->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextBrowserInteraction);
    QObject::connect(this->valueImage, SIGNAL(linkActivated(QString)), this, SLOT(showImage()));
    this->valueLaunchTime = new QLabel("");
    this->valueLaunchTime->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valuePublicIP = new QLabel("");
    this->valuePublicIP->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valuePrivateIP = new QLabel("");
    this->valuePrivateIP->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueVpc = new QLabel("");
    this->valueVpc->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextBrowserInteraction);
    QObject::connect(this->valueVpc, SIGNAL(linkActivated(QString)), this, SLOT(showVpc()));
    this->valueSubnet = new QLabel("");
    this->valueSubnet->setTextFormat(Qt::RichText);
    this->valueSubnet->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QObject::connect(this->valueSubnet, SIGNAL(linkActivated(QString)), this, SLOT(showSubnet()));
    this->valueSourceDestCheck = new QLabel("");
    this->valueSourceDestCheck->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueCfStackName = new QLabel("");
    this->valueCfStackName->setTextInteractionFlags(Qt::TextSelectableByMouse);
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
    this->gridLayout->addWidget(this->labelImage, 7, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueImage, 7, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelLaunchTime, 8, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueLaunchTime, 8, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelPublicIP, 9, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePublicIP, 9, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelPrivateIP, 10, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePrivateIP, 10, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelVpc, 11, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVpc, 11, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSubnet, 12, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSubnet, 12, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelSourceDestCheck, 13, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSourceDestCheck, 13, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelCfStackName, 14, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueCfStackName, 14, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelTags, 15, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueTags, 15, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSecurityGroups, 16, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSecurityGroups, 16, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelVirtualizationType, 17, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVirtualizationType, 17, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelArchitecture, 18, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueArchitecture, 18, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelHypervisor, 19, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHypervisor, 19, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(20, 1);
    this->gridLayout->setColumnStretch(2, 1);

    this->awsContent->setLayout(this->gridLayout);

    this->mainLayout = new QVBoxLayout();
    this->mainLayout->addWidget(this->widgetStack);
    this->mainLayout->setContentsMargins(0, 0, 0, 0);
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
    this->valueImage->setText(QString("%1 (<a href=\"http://localhost/\">View Image</a>)").arg(instance->formattedImage()));

    this->valueLaunchTime->setText(instance->launchTime);
    this->valuePublicIP->setText(instance->publicIP);
    this->valuePrivateIP->setText(instance->privateIP);
    this->valueVpc->setText(QString("%1 (<a href=\"http://localhost/\">View VPC</a>)").arg(instance->formattedVpc()));

    this->valueSubnet->setText(QString("%1 (<a href=\"http://localhost/\">View Subnet</a>)").arg(instance->formattedSubnet()));
    this->valueSourceDestCheck->setText(instance->sourceDestCheck);
    this->valueCfStackName->setText(instance->cfStackName);
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
            std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups = parseDescribeSecurityGroupsResponse(result, this->instance->region);

            this->securityGroupsDialog->updateData(securityGroups);
        } else if (result->responseType == "DescribeInstancesResponse") {
            std::cout << "Describe Instances" << std::endl;

            std::vector<std::shared_ptr<AWSInstance>> instances = parseDescribeInstancesResponse(result, this->instance->region);

            if (instances.size() == 1) {
                std::shared_ptr<AWSInstance> newInstance = instances.at(0);
                *(this->instance) = *newInstance;
                this->updateData(this->instance);
            } else if (instances.size() == 0) {
                // The instance was terminted some time ago and AWS already
                // deleted its data
                this->instance->status = "terminated";
                this->updateData(this->instance);
            } else {
                // AWS returns more than one instance. This should never happen!
                QMessageBox msgBox;
                msgBox.setText(tr("Failed to update Instance: AWS returned more than one instance!"));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
            }
        } else if (result->responseType == "DescribeSubnetsResponse") {
            std::vector<std::shared_ptr<AWSSubnet>> subnets = parseDescribeSubnetsResponse(result, this->instance->region);

            this->subnetDialog->updateData(subnets);
        } else if (result->responseType == "DescribeVpcsResponse") {
            std::vector<std::shared_ptr<AWSVpc>> vpcs = parseDescribeVpcsResponse(result, this->instance->region);

            this->vpcDialog->updateData(vpcs);
        } else if (result->responseType == "DescribeImagesResponse") {
            std::vector<std::shared_ptr<AWSImage>> images = parseDescribeImagesResponse(result, this->instance->region);

            this->imageDialog->updateData(images);
        }
    }

    delete result;
}

void AWSInfoWidget::showSecurityGroups()
{
    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->securityGroupsDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoWidget::showImage()
{
    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->imageDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoWidget::showVpc()
{
    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->vpcDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoWidget::showSubnet()
{
    this->awsConnector->setAccessKey(this->preferences->getAWSAccessKey());
    this->awsConnector->setSecretKey(this->preferences->getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->subnetDialog->showDialog(this->awsConnector, instance);
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
