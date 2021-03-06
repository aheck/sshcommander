/*****************************************************************************
 *
 * AWSConnector implements a simple and VERY minimal interface to talk to
 * the parts of Amazon's AWS API needed by this program.
 *
 * The API reference with descriptions of the requests can be found here:
 *
 * http://docs.aws.amazon.com/AWSEC2/latest/APIReference/Welcome.html
 *
 ****************************************************************************/

#ifndef AWSCONNECTOR_H
#define AWSCONNECTOR_H

#include <iostream>

#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

#include "AWSInstance.h"
#include "AWSSubnet.h"
#include "AWSVpc.h"

struct AWSResult
{
    bool isSuccess;
    QString errorString;
    int httpStatus;
    QString responseType;
    QString httpBody;
    QString userType; // a type defined by the user to identify a response
};

struct AWSFilter
{
    QString name;
    QStringList values;
};

class AWSConnector : public QObject
{
    Q_OBJECT

signals:
    void awsReplyReceived(AWSResult *result);

public:
    static const QStringList Regions;
    static const QStringList RegionNames;

    static const QString LOCATION_US_EAST_1;
    static const QString LOCATION_US_EAST_2;

    static const QString LOCATION_EU_WEST_1;
    static const QString LOCATION_EU_CENTRAL_1;

    static const QString AWS_DEFAULT_ENDPOINT;

    explicit AWSConnector();
    ~AWSConnector();

    void setAccessKey(QString accessKey);
    void setSecretKey(QString secretKey);
    void setRegion(QString region);

    void sendRequest(const QString action, const QString userType = "");
    void sendRequest(const QString action, QList<QString> &extraParams, const QString userType = "");

    // These methods execute actual AWS API calls. They have no return value
    // because the API is asynchronous. To receive replies the caller has to
    // connect to the awsReplyReceived signal.
    void describeInstances(const QString userType = "");
    void describeInstances(QList<QString> &instanceIds, const QString userType = "");
    void describeSecurityGroups(QList<QString> &groupIds, const QString userType = "");
    void describeSubnets(QList<QString> &subnetIds, const QString userType = "");
    void describeVpcs(const QString userType = "");
    void describeVpcs(QList<QString> &vpcIds, const QString userType = "");
    void describeImages(QList<QString> &imageIds, const QString userType = "");
    void describeRouteTables(const QString userType = "");
    void describeRouteTablesWithSubnetId(QString &subnetId, const QString userType = "");
    void describeRouteTableMain(QString &vpcId, const QString userType = "");

    void stopInstances(QList<QString> instanceIds);
    void startInstances(QList<QString> instanceIds);
    void rebootInstances(QList<QString> instanceIds);
    void terminateInstances(QList<QString> instanceIds);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    QByteArray sign(QByteArray key, QByteArray message);
    QByteArray getSignatureKey(QByteArray key, QByteArray dateStamp);

    QString accessKey;
    QString secretKey;
    QString region;
    QNetworkAccessManager networkManager;
};

#endif
