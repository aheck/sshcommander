#include <QApplication>
#include <QBuffer>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QSettings>
#include <QTableWidget>
#include <QToolBar>
#include <QWidget>
#include <QXmlStreamReader>

#include "AWSConnector.h"

#ifndef AWSWIDGET_H
#define AWSWIDGET_H

class AWSWidget : public QWidget
{
    Q_OBJECT

public slots:
    void connectToAWS();
    void handleAWSResult(AWSResult *result);

public:
    AWSWidget();
    ~AWSWidget();

private slots:
    void loadInstances();
    void changeRegion(QString region);

private:
    void readSettings();
    void saveAWSCredentials();
    QVector<AWSInstance*>* parseDescribeInstancesResult(AWSResult *result);

    QLineEdit *accessKeyLineEdit;
    QLineEdit *secretKeyLineEdit;
    QPushButton *awsLoginButton;
    AWSConnector *awsConnector;

    QString accessKey;
    QString secretKey;

    // before login the user sees the loginWidget afterwards the mainWidget with
    // a listing of AWS instances etc.
    QWidget *loginWidget;
    QWidget *mainWidget;

    QWidget *curWidget;

    QTableWidget *instanceTable;
    QToolBar *toolBar;
    QComboBox *regionComboBox;
    QString region;
    bool requestRunning;
    bool firstTryToLogin;
};

#endif
