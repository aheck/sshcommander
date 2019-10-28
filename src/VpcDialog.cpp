#include "VpcDialog.h"

VpcDialog::VpcDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(500);
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *caption = new QLabel("VPC Details");
    QFont font = caption->font();
    font.setPointSize(18);
    font.setBold(true);
    caption->setFont(font);
    caption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(caption);

    QFormLayout *formLayout = new QFormLayout();
    this->vpcIdLabel = new QLabel();
    this->vpcIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->nameLabel = new QLabel();
    this->nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->stateLabel = new QLabel();
    this->stateLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->cidrBlockLabel = new QLabel();
    this->cidrBlockLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->dhcpOptionsLabel = new QLabel();
    this->dhcpOptionsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->instanceTenancyLabel = new QLabel();
    this->instanceTenancyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->isDefaultLabel = new QLabel();
    this->isDefaultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    formLayout->addRow(tr("VPC ID: "), this->vpcIdLabel);
    formLayout->addRow(tr("Name: "), this->nameLabel);
    formLayout->addRow(tr("State: "), this->stateLabel);
    formLayout->addRow(tr("CIDR Block: "), this->cidrBlockLabel);
    formLayout->addRow(tr("DHCP options: "), this->dhcpOptionsLabel);
    formLayout->addRow(tr("Instance Tenancy: "), this->instanceTenancyLabel);
    formLayout->addRow(tr("Default VPC: "), this->isDefaultLabel);

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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

void VpcDialog::showDialog(AWSConnector *connector, const QString vpcId, const QString vpcName)
{
    if (vpcName.isEmpty()) {
        this->setWindowTitle("VPC " + vpcId);
    } else {
        this->setWindowTitle("VPC " + vpcName + "(" + vpcId + ")");
    }

    this->clear();

    if (!vpcId.isEmpty()) {
        QList<QString> vpcIds;
        vpcIds.append(vpcId);
        connector->describeVpcs(vpcIds, "VpcDialog");
    }

    this->exec();
}

void VpcDialog::showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance)
{
    QString title;
    if (instance->name.isEmpty()) {
        title = QString("VPC '%1' of instance %2").arg(instance->formattedVpc()).arg(instance->id);
    } else {
        title = QString("VPC '%1' of instance '%2' (%3)").arg(instance->formattedVpc()).arg(instance->name).arg(instance->id);
    }

    this->setWindowTitle(title);
    this->clear();

    if (!instance->vpcId.isEmpty()) {
        QList<QString> vpcIds;
        vpcIds.append(instance->vpcId);
        connector->describeVpcs(vpcIds, "VpcDialog");
    }

    this->exec();
}

void VpcDialog::updateData(std::vector<std::shared_ptr<AWSVpc>> vpcs)
{
    if (vpcs.size() == 0) {
        return;
    }

    auto vpc = vpcs.at(0);

    this->vpcIdLabel->setText(vpc->id);
    this->nameLabel->setText(vpc->name);
    this->stateLabel->setText(vpc->state);
    this->cidrBlockLabel->setText(vpc->cidrBlock);
    this->dhcpOptionsLabel->setText(vpc->dhcpOptionsId);
    this->instanceTenancyLabel->setText(vpc->instanceTenancy);
    this->isDefaultLabel->setText(vpc->isDefault ? "true" : "false");

    this->tagsViewer->updateData(vpc->tags);
}

void VpcDialog::clear()
{
    this->vpcIdLabel->setText("");
    this->nameLabel->setText("");
    this->stateLabel->setText("");
    this->cidrBlockLabel->setText("");
    this->dhcpOptionsLabel->setText("");
    this->instanceTenancyLabel->setText("");
    this->isDefaultLabel->setText("");

    this->tagsViewer->clear();
}
