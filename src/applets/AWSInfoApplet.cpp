#include "AWSInfoApplet.h"

AWSInfoApplet::AWSInfoApplet()
{
    this->securityGroupsDialog = new SecurityGroupsDialog();
    this->imageDialog = new ImageDialog();
    this->subnetDialog = new SubnetDialog();
    this->vpcDialog = new VpcDialog();

    this->awsConnector = new AWSConnector();
    QObject::connect(this->awsConnector, SIGNAL(awsReplyReceived(AWSResult*)), this, SLOT(handleAWSResult(AWSResult*)));

    this->awsPage = new QWidget();
    this->awsContent = new QWidget();
    this->awsContent->setObjectName("scrollAreaContent");

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

    this->valueInstanceId = new QLabel("");
    this->valueInstanceId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueName = new QLabel("");
    this->valueName->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueRegion = new QLabel("");
    this->valueRegion->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueAvailabilityZone = new QLabel("");
    this->valueAvailabilityZone->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueStatus = new QLabel("");
    this->valueStatus->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
    this->tagsViewer = new TagsViewWidget();
    this->tagsViewer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->valueSecurityGroups = new QLabel();
    this->valueSecurityGroups->setText("<a href=\"http://localhost/\" style=\"font-weight: bold;\">View Security Groups</a>");
    this->valueSecurityGroups->setTextFormat(Qt::RichText);
    this->valueSecurityGroups->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QObject::connect(this->valueSecurityGroups, SIGNAL(linkActivated(QString)), this, SLOT(showSecurityGroups()));
    this->valueVirtualizationType = new QLabel("");
    this->valueVirtualizationType->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueArchitecture = new QLabel("");
    this->valueArchitecture->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueHypervisor = new QLabel("");
    this->valueHypervisor->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QHBoxLayout *groupsLayout = new QHBoxLayout();
    QVBoxLayout *groupsLeftLayout = new QVBoxLayout();
    groupsLeftLayout->setAlignment(Qt::AlignTop);
    QVBoxLayout *groupsRightLayout = new QVBoxLayout();
    groupsRightLayout->setAlignment(Qt::AlignTop);

    groupsLayout->addLayout(groupsLeftLayout);
    groupsLayout->addLayout(groupsRightLayout);

    QGroupBox *generalGroup = new QGroupBox(tr("General"));
    generalGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFormLayout *generalLayout = new QFormLayout();

    generalLayout->addRow(tr("Instance ID:"), this->valueInstanceId);
    generalLayout->addRow(tr("Name:"), this->valueName);
    generalLayout->addRow(tr("Region:"), this->valueRegion);
    generalLayout->addRow(tr("Availability Zone:"), this->valueAvailabilityZone);
    generalLayout->addRow(tr("Status:"), this->valueStatus);
    generalLayout->addRow(tr("Keyname:"), this->valueKeyname);
    generalLayout->addRow(tr("Type:"), this->valueType);
    generalLayout->addRow(tr("Image (AMI):"), this->valueImage);
    generalLayout->addRow(tr("Launch Time:"), this->valueLaunchTime);
    generalLayout->addRow(tr("CloudFormation Stack:"), this->valueCfStackName);
    generalLayout->addRow(tr("Tags:"), this->tagsViewer);

    generalGroup->setLayout(generalLayout);
    groupsLeftLayout->addWidget(generalGroup);

    QGroupBox *networkGroup = new QGroupBox(tr("Network"));
    networkGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFormLayout *networkLayout = new QFormLayout();

    networkLayout->addRow(tr("Public IP:"), this->valuePublicIP);
    networkLayout->addRow(tr("Private IP:"), this->valuePrivateIP);
    networkLayout->addRow(tr("VPC:"), this->valueVpc);
    networkLayout->addRow(tr("Subnet:"), this->valueSubnet);
    networkLayout->addRow(tr("Source Dest Check"), this->valueSourceDestCheck);
    networkLayout->addRow(tr("Security Groups:"), this->valueSecurityGroups);

    networkGroup->setLayout(networkLayout);
    groupsRightLayout->addWidget(networkGroup);

    QGroupBox *miscGroup = new QGroupBox(tr("Misc"));
    miscGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFormLayout *miscLayout = new QFormLayout();

    miscLayout->addRow(tr("Virtualization Type:"), this->valueVirtualizationType);
    miscLayout->addRow(tr("Architecture:"), this->valueArchitecture);
    miscLayout->addRow(tr("Hypervisor:"), this->valueHypervisor);

    miscGroup->setLayout(miscLayout);
    groupsRightLayout->addWidget(miscGroup);

    this->awsContent->setLayout(groupsLayout);

    this->mainLayout = new QVBoxLayout();
    this->mainLayout->addWidget(this->awsPage);
    this->mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
}

AWSInfoApplet::~AWSInfoApplet()
{
    delete this->securityGroupsDialog;
    delete this->awsConnector;
}

const QString AWSInfoApplet::getDisplayName()
{
    return "AWS";
}

QIcon AWSInfoApplet::getIcon()
{
    return QIcon(":/images/connection-type-aws.svg");
}

void AWSInfoApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
    this->updateData(connEntry->awsInstance);
}

void AWSInfoApplet::updateData(std::shared_ptr<AWSInstance> newInstance)
{
    newInstance->copyResolvedReferences(this->instance);
    this->instance = newInstance;
    this->instance->resolveReferences();

    this->valueInstanceId->setText(this->instance->id);
    this->valueName->setText(this->instance->name);
    this->valueRegion->setText(this->instance->region);
    this->valueAvailabilityZone->setText(this->instance->availabilityZone);

    this->valueStatus->setText(this->instance->status);
    if (this->instance->status == "running") {
        this->valueStatus->setStyleSheet("QLabel { font-weight: bold; color: green;}");
    } else if (this->instance->status == "terminated") {
        this->valueStatus->setStyleSheet("QLabel { font-weight: bold; color : red;}");
    } else {
        this->valueStatus->setStyleSheet("QLabel { font-weight: bold; color : goldenrod;}");
    }
    this->valueKeyname->setText(this->instance->keyname);
    this->valueType->setText(this->instance->type);
    this->valueImage->setText(QString("%1 (<a href=\"http://localhost/\" style=\"font-weight: bold;\">View Image</a>)")
            .arg(this->instance->formattedImage()));

    this->valueLaunchTime->setText(this->instance->launchTime);
    this->valuePublicIP->setText(this->instance->publicIP);
    this->valuePrivateIP->setText(this->instance->privateIP);
    this->valueVpc->setText(QString("%1 (<a href=\"http://localhost/\" style=\"font-weight: bold;\">View VPC</a>)")
            .arg(this->instance->formattedVpc()));

    this->valueSubnet->setText(QString("%1 (<a href=\"http://localhost/\" style=\"font-weight: bold;\">View Subnet</a>)")
            .arg(this->instance->formattedSubnet()));
    this->valueSourceDestCheck->setText(this->instance->sourceDestCheck);
    this->valueCfStackName->setText(this->instance->cfStackName);
    this->valueVirtualizationType->setText(this->instance->virtualizationType);
    this->valueArchitecture->setText(this->instance->architecture);
    this->valueHypervisor->setText(this->instance->hypervisor);

    this->tagsViewer->updateData(this->instance->tags);
}

void AWSInfoApplet::handleAWSResult(AWSResult *result)
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
                this->updateData(newInstance);

                AWSCache &cache = AWSCache::getInstance();
                cache.updateInstances(newInstance->region, instances);
                emit awsInstancesUpdated();
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

            this->instance->resolveReferences();
            this->subnetDialog->updateData(subnets);

            AWSCache &cache = AWSCache::getInstance();
            cache.updateSubnets(this->instance->region, subnets);
        } else if (result->responseType == "DescribeRouteTablesResponse") {
            std::vector<std::shared_ptr<AWSRouteTable>> routeTables = parseDescribeRouteTablesResponse(result, this->instance->region);

            if (routeTables.size() == 0) {
                // First we try to get the routing table associated with the
                // subnet. If this request returns an empty set this means that
                // the subnet is implicitly associated with the main routing
                // table of the VPC. So we make a second request here to get the
                // VPCs main routing table.
                this->awsConnector->describeRouteTableMain(this->instance->vpcId);
            } else {
                this->subnetDialog->updateRouteTable(routeTables);
            }
        } else if (result->responseType == "DescribeVpcsResponse") {
            std::vector<std::shared_ptr<AWSVpc>> vpcs = parseDescribeVpcsResponse(result, this->instance->region);

            this->instance->resolveReferences();
            this->vpcDialog->updateData(vpcs);

            AWSCache &cache = AWSCache::getInstance();
            cache.updateVpcs(this->instance->region, vpcs);
        } else if (result->responseType == "DescribeImagesResponse") {
            std::vector<std::shared_ptr<AWSImage>> images = parseDescribeImagesResponse(result, this->instance->region);

            this->imageDialog->updateData(images);
        }
    }

    delete result;
}

void AWSInfoApplet::showSecurityGroups()
{
    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->securityGroupsDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoApplet::showImage()
{
    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->imageDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoApplet::showVpc()
{
    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->vpcDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoApplet::showSubnet()
{
    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    this->subnetDialog->showDialog(this->awsConnector, instance);
}

void AWSInfoApplet::reloadInstanceData()
{
    Preferences &preferences = Preferences::getInstance();

    this->awsConnector->setAccessKey(preferences.getAWSAccessKey());
    this->awsConnector->setSecretKey(preferences.getAWSSecretKey());
    this->awsConnector->setRegion(this->instance->region);

    QList<QString> instanceIds;
    instanceIds.append(instance->id);

    this->awsConnector->describeInstances(instanceIds);
}
