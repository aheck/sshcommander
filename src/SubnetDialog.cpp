#include "SubnetDialog.h"

SubnetDialog::SubnetDialog()
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *caption = new QLabel("Subnet Details");
    QFont font = caption->font();
    font.setPointSize(18);
    font.setBold(true);
    caption->setFont(font);
    caption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(caption);

    QFormLayout *formLayout = new QFormLayout();
    this->subnetIdLabel = new QLabel();
    this->subnetIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->nameLabel = new QLabel();
    this->nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->stateLabel = new QLabel();
    this->stateLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->vpcIdLabel = new QLabel();
    this->vpcIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->cidrBlockLabel = new QLabel();
    this->cidrBlockLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->availableIpAddressCountLabel = new QLabel();
    this->availableIpAddressCountLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->availabilityZoneLabel = new QLabel();
    this->availabilityZoneLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->defaultForAzLabel = new QLabel();
    this->defaultForAzLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->mapPublicIpOnLaunchLabel = new QLabel();
    this->mapPublicIpOnLaunchLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    formLayout->addRow(tr("Subnet ID: "), this->subnetIdLabel);
    formLayout->addRow(tr("Name: "), this->nameLabel);
    formLayout->addRow(tr("State: "), this->stateLabel);
    formLayout->addRow(tr("VPC ID: "), this->vpcIdLabel);
    formLayout->addRow(tr("CIDR Block: "), this->cidrBlockLabel);
    formLayout->addRow(tr("Available Addresses: "), this->availableIpAddressCountLabel);
    formLayout->addRow(tr("Availability Zone: "), this->availabilityZoneLabel);
    formLayout->addRow(tr("Default Subnet of AZ: "), this->defaultForAzLabel);
    formLayout->addRow(tr("Auto-assign Public IP: "), this->mapPublicIpOnLaunchLabel);

    layout->addLayout(formLayout);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(closeButton);

    this->setLayout(layout);
}

void SubnetDialog::showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance)
{
    QString title;
    if (instance->name.isEmpty()) {
        title = QString("Subnet '%1' of instance %2").arg(instance->formattedSubnet()).arg(instance->id);
    } else {
        title = QString("Subnet '%1' of instance '%2' (%3)").arg(instance->formattedSubnet()).arg(instance->name).arg(instance->id);
    }

    this->setWindowTitle(title);

    if (!instance->subnetId.isEmpty()) {
        QList<QString> subnets;
        subnets.append(instance->subnetId);
        connector->describeSubnets(subnets);
    }

    this->exec();
}

void SubnetDialog::updateData(std::vector<std::shared_ptr<AWSSubnet>> subnets)
{
    if (subnets.size() == 0) {
        return;
    }

    auto subnet = subnets.at(0);

    this->subnetIdLabel->setText(subnet->id);
    this->nameLabel->setText(subnet->name);
    this->stateLabel->setText(subnet->state);
    this->vpcIdLabel->setText(subnet->vpcId);
    this->cidrBlockLabel->setText(subnet->cidrBlock);
    this->availableIpAddressCountLabel->setText(subnet->availableIpAddressCount);
    this->availabilityZoneLabel->setText(subnet->availabilityZone);
    this->defaultForAzLabel->setText(subnet->defaultForAz ? "true" : "false");
    this->mapPublicIpOnLaunchLabel->setText(subnet->mapPublicIpOnLaunch ? "true" : "false");
}
