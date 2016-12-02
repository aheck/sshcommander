#ifndef AWSINFOWIDGET_H
#define AWSINFOWIDGET_H

#include <memory>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QStackedWidget>
#include <QWidget>

#include "AWSConnector.h"
#include "AWSResponseParsers.h"
#include "Preferences.h"
#include "SecurityGroupsDialog.h"

class AWSInfoWidget : public QWidget
{
    Q_OBJECT

public:
    AWSInfoWidget(Preferences *preferences);
    ~AWSInfoWidget();

    void update(std::shared_ptr<AWSInstance> instance);
    void setAWSEnabled(bool enabled);

public slots:
    void handleAWSResult(AWSResult *result);
    void showSecurityGroups();

private:
    Preferences *preferences;
    std::shared_ptr<AWSInstance> instance;
    AWSConnector *awsConnector;
    SecurityGroupsDialog *securityGroupsDialog;
    bool enabled;
    QScrollArea *scrollArea;
    QStackedWidget *widgetStack;
    QWidget *awsPage;
    QWidget *noAWSPage;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;

    QLabel *labelInstanceId;
    QLabel *labelName;
    QLabel *labelRegion;
    QLabel *labelStatus;
    QLabel *labelKeyname;
    QLabel *labelType;
    QLabel *labelImageId;
    QLabel *labelLaunchTime;
    QLabel *labelPublicIP;
    QLabel *labelPrivateIP;
    QLabel *labelSubnetId;
    QLabel *labelVpcId;
    QLabel *labelTags;
    QLabel *labelSecurityGroups;
    QLabel *labelVirtualizationType;
    QLabel *labelArchitecture;
    QLabel *labelHypervisor;

    QLabel *valueInstanceId;
    QLabel *valueName;
    QLabel *valueRegion;
    QLabel *valueStatus;
    QLabel *valueKeyname;
    QLabel *valueType;
    QLabel *valueImageId;
    QLabel *valueLaunchTime;
    QLabel *valuePublicIP;
    QLabel *valuePrivateIP;
    QLabel *valueSubnetId;
    QLabel *valueVpcId;
    QLabel *valueTags;
    QLabel *valueSecurityGroups;
    QLabel *valueVirtualizationType;
    QLabel *valueArchitecture;
    QLabel *valueHypervisor;
};

#endif
