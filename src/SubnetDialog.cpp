#include "SubnetDialog.h"

SubnetDialog::SubnetDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(500);
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

    QLabel *tagsCaption = new QLabel("Tags");
    font = tagsCaption->font();
    font.setPointSize(12);
    font.setBold(true);
    tagsCaption->setFont(font);
    tagsCaption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(tagsCaption);

    this->tagsViewer = new TagsViewWidget();
    layout->addWidget(this->tagsViewer);

    QLabel *routeTableCaption = new QLabel("Route Table");
    font = routeTableCaption->font();
    font.setPointSize(12);
    font.setBold(true);
    routeTableCaption->setFont(font);
    routeTableCaption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(routeTableCaption);

    this->routeTableViewer = new RouteTableViewWidget();
    layout->addWidget(this->routeTableViewer);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, &QPushButton::clicked, this, &SubnetDialog::reject);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

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
    this->clear();

    if (!instance->subnetId.isEmpty()) {
        QList<QString> subnets;
        subnets.append(instance->subnetId);
        connector->describeSubnets(subnets);
        connector->describeRouteTablesWithSubnetId(instance->subnetId);
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

    this->tagsViewer->updateData(subnet->tags);
}

void SubnetDialog::updateRouteTable(std::vector<std::shared_ptr<AWSRouteTable>> routeTables)
{
    if (routeTables.size() < 1) {
        return;
    }

    this->routeTableViewer->updateData(routeTables.at(0));
}

void SubnetDialog::clear()
{
    this->subnetIdLabel->setText("");
    this->nameLabel->setText("");
    this->stateLabel->setText("");
    this->vpcIdLabel->setText("");
    this->cidrBlockLabel->setText("");
    this->availableIpAddressCountLabel->setText("");
    this->availabilityZoneLabel->setText("");
    this->defaultForAzLabel->setText("");
    this->mapPublicIpOnLaunchLabel->setText("");

    this->tagsViewer->clear();
    this->routeTableViewer->clear();
}
