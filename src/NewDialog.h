#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <memory>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDirIterator>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>

#include "AWSConnector.h"
#include "globals.h"

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    NewDialog(bool editDialog = false);

    const QString getHostname();
    void setHostname(const QString hostname);
    const QString getUsername();
    void setUsername(const QString username);
    const QString getShortDescription();
    void setShortDescription(const QString shortDescription);
    const QString getSSHKey();
    void setSSHKey(const QString sshkey);
    int getPortNumber();
    void setPortNumber(int port);

    void setFocusOnUsername();

    int exec() override;

    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;

public slots:
    void selectKeyFile();
    void acceptDialog();
    void portCheckBoxStateChanged(bool checked);

private:
    QLineEdit *hostnameLineEdit;
    QLineEdit *usernameLineEdit;
    //QLineEdit *passwordLineEdit;
    QLineEdit *shortDescriptionLineEdit;
    QComboBox *sshkeyComboBox;
    QLineEdit *portLineEdit;
    QCheckBox *portCheckBox;
};

#endif
