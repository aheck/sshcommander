#include <iostream>

#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#ifndef AWSCONNECTOR_H
#define AWSCONNECTOR_H

struct AWSResult
{
    bool isSuccess;
    QString errorString;
    int httpStatus;
    QString httpBody;
};

struct AWSInstance
{
    QString id;
    QString status;
    QString type;
    QString imageId;
    QString launchTime;
    QString publicIP;
    QString privateIP;
    QString subnetId;
    QString vpcId;
    QString virtualizationType;
    QString architecture;
    QString hypervisor;
};

class AWSConnector : public QObject
{
    Q_OBJECT

signals:
    void awsReplyReceived(AWSResult *result);

public:
    static const QStringList Regions;

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

    // These methods execute actual AWS API calls. They have no return value
    // because the API is asynchronous. To receive replies the caller has to
    // connect to the awsReplyReceived signal.
    void describeInstances();

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
