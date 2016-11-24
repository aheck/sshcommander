#include "AWSInfoWidget.h"

AWSInfoWidget::AWSInfoWidget()
{
    this->enabled = false;

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

    this->gridLayout->addWidget(this->labelVirtualizationType, 12, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueVirtualizationType, 12, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelArchitecture, 13, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueArchitecture, 13, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelHypervisor, 14, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHypervisor, 14, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(15, 1);
    this->gridLayout->setColumnStretch(2, 1);

    this->awsPage->setLayout(this->gridLayout);

    this->mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);
    this->mainLayout->addWidget(this->widgetStack);
}

AWSInfoWidget::~AWSInfoWidget()
{

}

void AWSInfoWidget::update(const AWSInstance &instance)
{
    this->valueInstanceId->setText(instance.id);
    this->valueName->setText(instance.name);
    this->valueRegion->setText(instance.region);
    this->valueStatus->setText(instance.status);
    this->valueKeyname->setText(instance.keyname);

    this->valueType->setText(instance.type);
    this->valueImageId->setText(instance.imageId);
    this->valueLaunchTime->setText(instance.launchTime);
    this->valuePublicIP->setText(instance.publicIP);

    this->valuePrivateIP->setText(instance.privateIP);
    this->valueSubnetId->setText(instance.subnetId);
    this->valueVpcId->setText(instance.vpcId);
    this->valueVirtualizationType->setText(instance.virtualizationType);

    this->valueArchitecture->setText(instance.architecture);
    this->valueHypervisor->setText(instance.hypervisor);
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
