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
    int httpStatus;
    QString replyBody;
};

class AWSConnector : QObject
{
    Q_OBJECT

public:
    static const QString LOCATION_US_EAST_1;
    static const QString LOCATION_US_EAST_2;

    static const QString LOCATION_EU_WEST_1;
    static const QString LOCATION_EU_CENTRAL_1;

    static const QString AWS_DEFAULT_ENDPOINT;

    AWSConnector();
    ~AWSConnector();

    void setAccessKey(QString accessKey);
    void setSecretKey(QString secretKey);
    void setRegion(QString region);

    AWSResult* describeInstances();

public slots:
    void replyFinished (QNetworkReply *reply);

private:
    QByteArray sign(QByteArray key, QByteArray message);
    QByteArray getSignatureKey(QByteArray key, QByteArray dateStamp);

    QString accessKey;
    QString secretKey;
    QString region;
    QNetworkAccessManager networkManager;
};

#endif
