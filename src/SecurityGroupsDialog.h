#ifndef SECURITYGROUPSDIALOG_H
#define SECURITYGROUPSDIALOG_H

#include <memory>

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include "AWSConnector.h"
#include "AWSInstance.h"
#include "AWSSecurityGroup.h"

class SecurityGroupsDialog : public QDialog
{
    Q_OBJECT

public:
    SecurityGroupsDialog();

    void showDialog(AWSConnector *connector, std::shared_ptr<AWSInstance> instance);
    void updateData(QVector<std::shared_ptr<AWSSecurityGroup>> securityGroups);

private:
    QListWidget *list;
};

#endif
