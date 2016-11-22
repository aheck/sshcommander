#ifndef NEWDIALOG_H
#define NEWDIALOG_H

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
    QLineEdit *sshkeyLineEdit;
    QLineEdit *portLineEdit;

    bool isAwsInstance;
    AWSInstance awsInstance;

public slots:
    void selectKeyFile();
    void acceptDialog();
    void portCheckBoxStateChanged(bool checked);
};

#endif
