/*****************************************************************************
 *
 * SecurityGroupsDialog is the dialog used to show AWS security groups.
 *
 ****************************************************************************/

#ifndef SECURITYGROUPSDIALOG_H
#define SECURITYGROUPSDIALOG_H

#include <memory>

#include <QDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "AWSSecurityGroup.h"

class SecurityGroupsDialog : public QDialog
{
    Q_OBJECT

public:
    SecurityGroupsDialog();

    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups);
    void clear();

private:
    QTableWidget *table;
};

#endif
