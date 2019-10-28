/*****************************************************************************
 *
 * VpcDialog is the dialog used to show AWS VPCs.
 *
 ****************************************************************************/

#ifndef VPCDIALOG_H
#define VPCDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "AWS/AWSConnector.h"
#include "AWS/AWSInstance.h"
#include "AWS/AWSVpc.h"
#include "TagsViewWidget.h"

class VpcDialog : public QDialog
{
    Q_OBJECT

public:
    VpcDialog(QWidget *parent = nullptr);

    void showDialog(AWSConnector *connector, const QString vpcId, const QString vpcName);
    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(std::vector<std::shared_ptr<AWSVpc>> vpc);
    void clear();

private:
    QLabel *vpcIdLabel;
    QLabel *nameLabel;
    QLabel *stateLabel;
    QLabel *cidrBlockLabel;
    QLabel *dhcpOptionsLabel;
    QLabel *instanceTenancyLabel;
    QLabel *isDefaultLabel;

    TagsViewWidget *tagsViewer;
};

#endif
