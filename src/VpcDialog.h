#ifndef VPCDIALOG_H
#define VPCDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "AWSVpc.h"

class VpcDialog : public QDialog
{
    Q_OBJECT

public:
    VpcDialog();

    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(std::vector<std::shared_ptr<AWSVpc>> vpc);

private:
    QLabel *vpcIdLabel;
    QLabel *nameLabel;
    QLabel *stateLabel;
    QLabel *cidrBlockLabel;
    QLabel *dhcpOptionsLabel;
    QLabel *instanceTenancyLabel;
    QLabel *isDefaultLabel;
};

#endif
