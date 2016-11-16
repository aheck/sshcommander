#ifndef AWSINFOWIDGET_H
#define AWSINFOWIDGET_H

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QWidget>

#include "AWSConnector.h"

class AWSInfoWidget : public QWidget
{
    Q_OBJECT

public:
    AWSInfoWidget();
    ~AWSInfoWidget();

    void update(const AWSInstance &instance);
    void setAWSEnabled(bool enabled);

private:
    bool enabled;
    QScrollArea *scrollArea;
    QStackedWidget *widgetStack;
    QWidget *awsPage;
    QWidget *noAWSPage;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;

    QLabel *labelInstanceId;
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
    QLabel *labelVirtualizationType;
    QLabel *labelArchitecture;
    QLabel *labelHypervisor;

    QLabel *valueInstanceId;
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
    QLabel *valueVirtualizationType;
    QLabel *valueArchitecture;
    QLabel *valueHypervisor;
};

#endif
