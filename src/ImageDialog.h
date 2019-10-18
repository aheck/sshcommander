/*****************************************************************************
 *
 * ImageDialog is the dialog used to show details about an AWS image.
 *
 ****************************************************************************/

#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "AWS/AWSConnector.h"
#include "AWS/AWSInstance.h"
#include "AWS/AWSImage.h"
#include "TagsViewWidget.h"

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    ImageDialog();

    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(std::vector<std::shared_ptr<AWSImage>> image);
    void clear();

private:
    QLabel *imageIdLabel;
    QLabel *locationLabel;
    QLabel *stateLabel;
    QLabel *ownerIdLabel;
    QLabel *isPublicLabel;
    QLabel *architectureLabel;
    QLabel *typeLabel;
    QLabel *kernelIdLabel;
    QLabel *ramdiskIdLabel;
    QLabel *ownerAliasLabel;
    QLabel *nameLabel;
    QLabel *descriptionLabel;
    QLabel *rootDeviceTypeLabel;
    QLabel *rootDeviceNameLabel;
    QLabel *virtualizationTypeLabel;
    QLabel *hypervisorLabel;
    QLabel *creationDateLabel;

    TagsViewWidget *tagsViewer;
};

#endif
