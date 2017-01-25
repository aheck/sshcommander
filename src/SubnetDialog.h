#ifndef SUBNETDIALOG_H
#define SUBNETDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "AWSSubnet.h"
#include "TagsViewWidget.h"

class SubnetDialog : public QDialog
{
    Q_OBJECT

public:
    SubnetDialog();

    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(std::vector<std::shared_ptr<AWSSubnet>> subnets);
    void clear();

private:
    QLabel *subnetIdLabel;
    QLabel *nameLabel;
    QLabel *stateLabel;
    QLabel *vpcIdLabel;
    QLabel *cidrBlockLabel;
    QLabel *availableIpAddressCountLabel;
    QLabel *availabilityZoneLabel;
    QLabel *defaultForAzLabel;
    QLabel *mapPublicIpOnLaunchLabel;

    TagsViewWidget *tagsViewer;
};

#endif
