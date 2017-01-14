#ifndef AWSINFOWIDGET_H
#define AWSINFOWIDGET_H

#include <memory>

#include <QApplication>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QStackedWidget>
#include <QToolBar>
#include <QWidget>

#include "AWSConnector.h"
#include "AWSResponseParsers.h"
#include "DisabledWidget.h"
#include "Preferences.h"
#include "SecurityGroupsDialog.h"
#include "SubnetDialog.h"
#include "VpcDialog.h"

class AWSInfoWidget : public QWidget
{
    Q_OBJECT

public:
    AWSInfoWidget(Preferences *preferences);
    ~AWSInfoWidget();

    void updateData(std::shared_ptr<AWSInstance> instance);
    void setAWSEnabled(bool enabled);

public slots:
    void handleAWSResult(AWSResult *result);
    void showSecurityGroups();
    void showVpc();
    void showSubnet();
    void reloadInstanceData();

private:
    Preferences *preferences;
    std::shared_ptr<AWSInstance> instance;
    AWSConnector *awsConnector;
    SecurityGroupsDialog *securityGroupsDialog;
    SubnetDialog *subnetDialog;
    VpcDialog *vpcDialog;
    bool enabled;
    QScrollArea *scrollArea;
    QStackedWidget *widgetStack;
    DisabledWidget * disabledWidget;
    QWidget *awsPage;
    QWidget *awsContent;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;
    QToolBar *toolBar;

    QLabel *labelInstanceId;
    QLabel *labelName;
    QLabel *labelRegion;
    QLabel *labelAvailabilityZone;
    QLabel *labelStatus;
    QLabel *labelKeyname;
    QLabel *labelType;
    QLabel *labelImageId;
    QLabel *labelLaunchTime;
    QLabel *labelPublicIP;
    QLabel *labelPrivateIP;
    QLabel *labelVpc;
    QLabel *labelSubnet;
    QLabel *labelSourceDestCheck;
    QLabel *labelCfStackName;
    QLabel *labelTags;
    QLabel *labelSecurityGroups;
    QLabel *labelVirtualizationType;
    QLabel *labelArchitecture;
    QLabel *labelHypervisor;

    QLabel *valueInstanceId;
    QLabel *valueName;
    QLabel *valueRegion;
    QLabel *valueAvailabilityZone;
    QLabel *valueStatus;
    QLabel *valueKeyname;
    QLabel *valueType;
    QLabel *valueImageId;
    QLabel *valueLaunchTime;
    QLabel *valuePublicIP;
    QLabel *valuePrivateIP;
    QLabel *valueCfStackName;
    QLabel *valueVpc;
    QLabel *valueSubnet;
    QLabel *valueSourceDestCheck;
    QLabel *valueTags;
    QLabel *valueSecurityGroups;
    QLabel *valueVirtualizationType;
    QLabel *valueArchitecture;
    QLabel *valueHypervisor;
};

#endif
