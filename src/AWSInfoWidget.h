#ifndef AWSINFOWIDGET_H
#define AWSINFOWIDGET_H

#include <memory>

#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "AWSConnector.h"
#include "AWSResponseParsers.h"
#include "DisabledWidget.h"
#include "ImageDialog.h"
#include "Preferences.h"
#include "SecurityGroupsDialog.h"
#include "SubnetDialog.h"
#include "TagsViewWidget.h"
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
    void showImage();
    void showVpc();
    void showSubnet();
    void reloadInstanceData();

private:
    Preferences *preferences;
    std::shared_ptr<AWSInstance> instance;
    AWSConnector *awsConnector;
    SecurityGroupsDialog *securityGroupsDialog;
    ImageDialog *imageDialog;
    SubnetDialog *subnetDialog;
    VpcDialog *vpcDialog;
    bool enabled;
    QScrollArea *scrollArea;
    QStackedWidget *widgetStack;
    DisabledWidget * disabledWidget;
    QWidget *awsPage;
    QWidget *awsContent;
    QVBoxLayout *mainLayout;
    QToolBar *toolBar;
    TagsViewWidget *tagsViewer;

    QLabel *valueInstanceId;
    QLabel *valueName;
    QLabel *valueRegion;
    QLabel *valueAvailabilityZone;
    QLabel *valueStatus;
    QLabel *valueKeyname;
    QLabel *valueType;
    QLabel *valueImage;
    QLabel *valueLaunchTime;
    QLabel *valuePublicIP;
    QLabel *valuePrivateIP;
    QLabel *valueCfStackName;
    QLabel *valueVpc;
    QLabel *valueSubnet;
    QLabel *valueSourceDestCheck;
    QLabel *valueSecurityGroups;
    QLabel *valueVirtualizationType;
    QLabel *valueArchitecture;
    QLabel *valueHypervisor;
};

#endif
