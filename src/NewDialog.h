#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <memory>

#include <QCheckBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "AWSConnector.h"
#include "globals.h"

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    NewDialog();
    ~NewDialog();

    int getPortNumber();

    QLineEdit *hostnameLineEdit;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *shortDescriptionLineEdit;
    QLineEdit *sshkeyLineEdit;
    QLineEdit *portLineEdit;

    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;

public slots:
    void selectKeyFile();
    void acceptDialog();
    void portCheckBoxStateChanged(bool checked);
};

#endif
