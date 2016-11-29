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

struct AWSResult
{
    bool isSuccess;
    QString errorString;
    int httpStatus;
    QString responseType;
    QString httpBody;
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

    void sendRequest(const QString action);
    void sendRequest(const QString action, QList<QString> &extraParams);

    // These methods execute actual AWS API calls. They have no return value
    // because the API is asynchronous. To receive replies the caller has to
    // connect to the awsReplyReceived signal.
    void describeInstances();
    void describeSecurityGroups(QList<QString> groupIds);

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
