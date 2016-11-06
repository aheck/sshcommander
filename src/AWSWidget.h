#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QSettings>
#include <QTableView>
#include <QToolBar>
#include <QWidget>
#include <QXmlStreamReader>

#include "AWSConnector.h"
#include "InstanceItemModel.h"

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

signals:
    void newConnection(const AWSInstance &instance);

private slots:
    void loadInstances();
    void connectToInstance();
    void changeRegion(QString region);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void readSettings();
    void saveAWSCredentials();
    QVector<AWSInstance*> parseDescribeInstancesResult(AWSResult *result);

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

    QTableView *instanceTable;
    InstanceItemModel *instanceModel;
    QToolBar *toolBar;
    QComboBox *regionComboBox;
    QString region;
    bool requestRunning;
    bool firstTryToLogin;
    QAction *connectButton;
};

#endif
