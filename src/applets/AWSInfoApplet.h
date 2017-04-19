#ifndef AWSINFOAPPLET_H
#define AWSINFOAPPLET_H

#include <memory>

#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "Applet.h"
#include "AWSConnector.h"
#include "AWSResponseParsers.h"
#include "ImageDialog.h"
#include "Preferences.h"
#include "SecurityGroupsDialog.h"
#include "SubnetDialog.h"
#include "TagsViewWidget.h"
#include "VpcDialog.h"

class AWSInfoApplet : public Applet
{
    Q_OBJECT

public:
    AWSInfoApplet();
    ~AWSInfoApplet();

    virtual const QString getDisplayName() override;
    virtual QIcon getIcon() override;
    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry) override;

    void updateData(std::shared_ptr<AWSInstance> newInstance);

public slots:
    void handleAWSResult(AWSResult *result);
    void showSecurityGroups();
    void showImage();
    void showVpc();
    void showSubnet();
    void reloadInstanceData();

signals:
    void awsInstancesUpdated();

private:
    std::shared_ptr<AWSInstance> instance;
    AWSConnector *awsConnector;
    SecurityGroupsDialog *securityGroupsDialog;
    ImageDialog *imageDialog;
    SubnetDialog *subnetDialog;
    VpcDialog *vpcDialog;
    QScrollArea *scrollArea;
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
