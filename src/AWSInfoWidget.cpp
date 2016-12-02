#include "AWSInfoWidget.h"

AWSInfoWidget::AWSInfoWidget(Preferences *preferences)
{
    this->preferences = preferences;

    this->enabled = false;

    this->securityGroupsDialog = new SecurityGroupsDialog();

    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->awsPage = new QWidget();
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

    this->scrollArea = new QScrollArea();
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setWidget(this->awsPage);

    this->widgetStack = new QStackedWidget();
    this->widgetStack->addWidget(this->noAWSPage);
    this->widgetStack->addWidget(this->scrollArea);

    this->labelInstanceId = new QLabel("Instance ID:");
    this->labelName = new QLabel("Name:");
    this->labelRegion = new QLabel("Region:");
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
    this->gridLayout->addWidget(this->labelStatus, 3, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueStatus, 3, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelKeyname, 4, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueKeyname, 4, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelType, 5, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueType, 5, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelImageId, 6, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueImageId, 6, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelLaunchTime, 7, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueLaunchTime, 7, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelPublicIP, 8, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePublicIP, 8, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelPrivateIP, 9, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valuePrivateIP, 9, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSubnetId, 10, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSubnetId, 10, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelVpcId, 11, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVpcId, 11, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelTags, 12, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueTags, 12, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelSecurityGroups, 13, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSecurityGroups, 13, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelVirtualizationType, 14, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVirtualizationType, 14, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelArchitecture, 15, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueArchitecture, 15, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelHypervisor, 16, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHypervisor, 16, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(17, 1);
    this->gridLayout->setColumnStretch(2, 1);

    this->awsPage->setLayout(this->gridLayout);

    this->mainLayout = new QVBoxLayout();
    this->mainLayout->addWidget(this->widgetStack);
    this->setLayout(mainLayout);
}

AWSInfoWidget::~AWSInfoWidget()
{
    delete this->securityGroupsDialog;
    delete this->awsConnector;
}

void AWSInfoWidget::update(std::shared_ptr<AWSInstance> instance)
{
    this->instance = instance;

    this->valueInstanceId->setText(instance->id);
    this->valueName->setText(instance->name);
    this->valueRegion->setText(instance->region);
    this->valueStatus->setText(instance->status);
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
