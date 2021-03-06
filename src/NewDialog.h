/*****************************************************************************
 *
 * NewDialog is the dialog which is shown when the user wants to create a
 * new SSH connection.
 *
 * If the connection is created successfully it is added to the list on the
 * left-hand side of the UI.
 *
 ****************************************************************************/

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

#include "AWS/AWSConnector.h"
#include "globals.h"

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    NewDialog(QWidget* parent = nullptr, bool editDialog = false);

    const QString getHostname();
    void setHostname(const QString hostname);
    const QString getUsername();
    void setUsername(const QString username);
    const QString getShortDescription();
    void setShortDescription(const QString shortDescription);
    const QString getPassword();
    void setPassword(const QString &password);
    const QString getSSHKey();
    void setSSHKey(const QString sshkey);
    int getPortNumber();
    void setPortNumber(int port);
    QStringList getHopHosts();
    QStringList getHopUsernames();
    QStringList getHopSSHKeys();
    void setHopChecked(bool checked);
    void addHopHost(QString name, QString ip);
    void updateSSHKeys();
    void clear();

    void setFocusOnUsername();

    static const QString findSSHKey(const QString keyname);

    bool isAwsInstance;
    std::shared_ptr<AWSInstance> awsInstance;

public slots:
    void selectKeyFileDefault();
    void selectKeyFileHop();
    void selectKeyFile(QComboBox *targetComboBox);
    void acceptDialog();

private slots:
    void portCheckBoxStateChanged(bool checked);
    void hopCheckBoxStateChanged(bool checked);

private:
    QFormLayout *formLayout;
    QLineEdit *hostnameLineEdit;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *shortDescriptionLineEdit;
    QComboBox *sshkeyComboBox;
    QLineEdit *portLineEdit;
    QCheckBox *portCheckBox;
    QCheckBox *hopCheckBox;
    QComboBox *hopHostBox;
    QLineEdit *hopUsernameLineEdit;
    QComboBox *hopSSHKeyComboBox;
    QToolButton *hopFileButton;
    QHBoxLayout *hopFileLayout;
};

#endif
