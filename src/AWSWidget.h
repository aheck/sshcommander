/*****************************************************************************
 *
 * AWSWidget implements the UI the user can use to explore his instances
 * running in Amazon's EC2 service.
 *
 ****************************************************************************/

#ifndef AWSWIDGET_H
#define AWSWIDGET_H

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QFormLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QSettings>
#include <QTableView>
#include <QToolBar>
#include <QWidget>

#include "AWSConnector.h"
#include "AWSResponseParsers.h"
#include "InstanceItemModel.h"
#include "Preferences.h"
#include "SecurityGroupsDialog.h"
#include "TagsDialog.h"

class AWSWidget : public QWidget
{
    Q_OBJECT

public slots:
    void connectToAWS();
    void handleAWSResult(AWSResult *result);
    void loadData();

public:
    AWSWidget(Preferences *preferences);

    QString getRegion() const;
    void setRegion(const QString region);

signals:
    void newConnection(std::shared_ptr<AWSInstance> instance,
            std::vector<std::shared_ptr<AWSInstance>> vpcNeighbours,
            bool toPrivateIP);
    void awsInstancesUpdated();

private slots:
    void doubleClicked(const QModelIndex &index);
    void connectToPublicIP();
    void connectToPrivateIP();
    void changeRegion(QString regionText);
    void changeVpc(int index);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void showInstanceContextMenu(QPoint pos);
    void showSecurityGroups();
    void showTags();
    void searchForText(const QString &text);
    void copyItemToClipboard();
    void clearVpcComboBox();
    void selectVpc();

    void stopInstance();
    void startInstance();
    void rebootInstance();
    void terminateInstance();

private:
    void updateNumberOfInstances();
    std::shared_ptr<AWSInstance> getSelectedInstance();
    void connectToInstance(bool toPrivateIP);
    void updateVpcs(std::vector<std::shared_ptr<AWSVpc>> &vpcs);

    Preferences *preferences;

    QLineEdit *accessKeyLineEdit;
    QLineEdit *secretKeyLineEdit;
    QPushButton *awsLoginButton;
    AWSConnector *awsConnector;

    // before login the user sees the loginWidget afterwards the mainWidget with
    // a listing of AWS instances etc.
    QWidget *loginWidget;
    QWidget *mainWidget;

    QWidget *curWidget;

    QTableView *instanceTable;
    InstanceItemModel *instanceModel;
    QToolBar *toolBar;
    QToolBar *vpcToolBar;
    QLineEdit *searchLineEdit;
    QComboBox *regionComboBox;
    QString selectedVpcId;
    QComboBox *vpcComboBox;
    QString region;
    bool requestRunning;
    bool firstTryToLogin;
    QAction *connectButton;
    QLabel *instanceNumLabel;
    SecurityGroupsDialog *securityGroupsDialog;
    TagsDialog *tagsDialog;

    bool updatingVpcs;

    // Value under the mouse cursor when the user right clicks. This value
    // might be copied to the clipboard if the user selects the corresponding
    // context menu action.
    QString clipboardCandidate;

    QString vpcIdCandidate;
};

#endif
