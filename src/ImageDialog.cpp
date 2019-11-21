#include "ImageDialog.h"

ImageDialog::ImageDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(500);
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *caption = new QLabel("Image (AMI) Details");
    QFont font = caption->font();
    font.setPointSize(18);
    font.setBold(true);
    caption->setFont(font);
    caption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(caption);

    QFormLayout *formLayout = new QFormLayout();
    this->imageIdLabel = new QLabel();
    this->imageIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->locationLabel = new QLabel();
    this->locationLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->stateLabel = new QLabel();
    this->stateLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->ownerIdLabel = new QLabel();
    this->ownerIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->isPublicLabel = new QLabel();
    this->isPublicLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->architectureLabel = new QLabel();
    this->architectureLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->typeLabel = new QLabel();
    this->typeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->kernelIdLabel = new QLabel();
    this->kernelIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->ramdiskIdLabel = new QLabel();
    this->ramdiskIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->ownerAliasLabel = new QLabel();
    this->ownerAliasLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->nameLabel = new QLabel();
    this->nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->descriptionLabel = new QLabel();
    this->descriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->rootDeviceTypeLabel = new QLabel();
    this->rootDeviceTypeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->rootDeviceNameLabel = new QLabel();
    this->rootDeviceNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->virtualizationTypeLabel = new QLabel();
    this->virtualizationTypeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->hypervisorLabel = new QLabel();
    this->hypervisorLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->creationDateLabel = new QLabel();
    this->creationDateLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    formLayout->addRow(tr("Image ID: "), this->imageIdLabel);
    formLayout->addRow(tr("Name: "), this->nameLabel);
    formLayout->addRow(tr("Description: "), this->descriptionLabel);
    formLayout->addRow(tr("Location: "), this->locationLabel);
    formLayout->addRow(tr("State: "), this->stateLabel);
    formLayout->addRow(tr("Owner ID: "), this->ownerIdLabel);
    formLayout->addRow(tr("Is Public: "), this->isPublicLabel);
    formLayout->addRow(tr("Architecture: "), this->architectureLabel);
    formLayout->addRow(tr("Image Type: "), this->typeLabel);
    formLayout->addRow(tr("Kernel ID: "), this->kernelIdLabel);
    formLayout->addRow(tr("Ramdisk ID: "), this->ramdiskIdLabel);
    formLayout->addRow(tr("Owner Alias: "), this->ownerAliasLabel);
    formLayout->addRow(tr("Root Device Type: "), this->rootDeviceTypeLabel);
    formLayout->addRow(tr("Root Device Name: "), this->rootDeviceNameLabel);
    formLayout->addRow(tr("Virtualization Type: "), this->virtualizationTypeLabel);
    formLayout->addRow(tr("Hypervisor: "), this->hypervisorLabel);
    formLayout->addRow(tr("Creation Date: "), this->creationDateLabel);

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
    QObject::connect(closeButton, &QPushButton::clicked, this, &ImageDialog::reject);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

void ImageDialog::showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance)
{
    QString title;
    if (instance->name.isEmpty()) {
        title = QString("Image '%1' of instance %2").arg(instance->formattedImage()).arg(instance->id);
    } else {
        title = QString("Image '%1' of instance '%2' (%3)").arg(instance->formattedImage()).arg(instance->name).arg(instance->id);
    }

    this->setWindowTitle(title);
    this->clear();

    if (!instance->imageId.isEmpty()) {
        QList<QString> imageIds;
        imageIds.append(instance->imageId);
        connector->describeImages(imageIds);
    }

    this->exec();
}

void ImageDialog::updateData(std::vector<std::shared_ptr<AWSImage>> images)
{
    if (images.size() == 0) {
        return;
    }

    auto image = images.at(0);

    this->imageIdLabel->setText(image->id);
    this->locationLabel->setText(image->location);
    this->stateLabel->setText(image->state);
    this->ownerIdLabel->setText(image->ownerId);
    this->isPublicLabel->setText(image->isPublic ? "true" : "false");
    this->architectureLabel->setText(image->architecture);
    this->typeLabel->setText(image->type);
    this->kernelIdLabel->setText(image->kernelId);
    this->ramdiskIdLabel->setText(image->ramdiskId);
    this->ownerAliasLabel->setText(image->ownerAlias);
    this->nameLabel->setText(image->name);
    this->descriptionLabel->setText(image->description);
    this->rootDeviceTypeLabel->setText(image->rootDeviceType);
    this->rootDeviceNameLabel->setText(image->rootDeviceName);
    this->virtualizationTypeLabel->setText(image->virtualizationType);
    this->hypervisorLabel->setText(image->hypervisor);
    this->creationDateLabel->setText(image->creationDate);

    this->tagsViewer->updateData(image->tags);
}

void ImageDialog::clear()
{
    this->imageIdLabel->setText("");
    this->locationLabel->setText("");
    this->stateLabel->setText("");
    this->ownerIdLabel->setText("");
    this->isPublicLabel->setText("");
    this->architectureLabel->setText("");
    this->typeLabel->setText("");
    this->kernelIdLabel->setText("");
    this->ramdiskIdLabel->setText("");
    this->ownerAliasLabel->setText("");
    this->nameLabel->setText("");
    this->descriptionLabel->setText("");
    this->rootDeviceTypeLabel->setText("");
    this->rootDeviceNameLabel->setText("");
    this->virtualizationTypeLabel->setText("");
    this->hypervisorLabel->setText("");
    this->creationDateLabel->setText("");

    this->tagsViewer->clear();
}
