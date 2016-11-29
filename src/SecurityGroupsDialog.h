#ifndef SECURITYGROUPSDIALOG_H
#define SECURITYGROUPSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class SecurityGroupsDialog : public QDialog
{
    Q_OBJECT

public:
    SecurityGroupsDialog();

    QListWidget *list;
};

#endif
