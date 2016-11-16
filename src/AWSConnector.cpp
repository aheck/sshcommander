#include "AWSConnector.h"

const QStringList AWSConnector::Regions = QStringList()
    << "us-east-1" << "us-east-2" << "us-west-1" << "us-west-2" << "eu-west-1"
    << "eu-central-1" << "ap-northeast-1" << "ap-northeast-2"
    << "ap-southeast-1" << "ap-southeast-2" << "ap-south-1" << "sa-east-1";

const QString AWSConnector::LOCATION_US_EAST_1 = "us-east-1";
const QString AWSConnector::LOCATION_US_EAST_2 = "us-east-2";

const QString AWSConnector::LOCATION_EU_WEST_1 = "eu-west-1";
const QString AWSConnector::LOCATION_EU_CENTRAL_1 = "eu-central-1";

AWSConnector::AWSConnector()
{
    connect(&this->networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

AWSConnector::~AWSConnector()
{

}

QByteArray AWSConnector::sign(QByteArray key, QByteArray message)
{
    return QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
}

QByteArray AWSConnector::getSignatureKey(QByteArray key, QByteArray dateStamp)
{
    QByteArray keyDate = this->sign(QByteArray("AWS4").append(key), dateStamp);
    QByteArray keyRegion = this->sign(keyDate, this->region.toUtf8());
    QByteArray keyService = this->sign(keyRegion, QByteArray("ec2"));
    QByteArray result = this->sign(keyService, QByteArray("aws4_request"));

    return result;
}

void AWSConnector::setAccessKey(QString accessKey)
{
    this->accessKey = accessKey;
}

void AWSConnector::setSecretKey(QString secretKey)
{
    this->secretKey = secretKey;
}

void AWSConnector::setRegion(QString region)
{
    this->region = region;
}

void AWSConnector::describeInstances()
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    // Request Params
    QString paramAction = "DescribeInstances";
    QString paramVersion = "2016-09-15";
    QString paramAmzAlgorithm = "AWS4-HMAC-SHA256";
    QString paramAmzDate = now.toString("yyyyMMddTHHmmssZ");
    QString paramAmzSignedHeaders = "host;x-amz-date";

    QString method = "GET";
    QString canonicalUri = "/";
    QByteArray canonicalQueryString;
    canonicalQueryString += QString("Action=").toUtf8() + QUrl::toPercentEncoding(paramAction);
    canonicalQueryString += '&';
    canonicalQueryString += QString("Version=").toUtf8() + QUrl::toPercentEncoding(paramVersion);

    QString host = "ec2." + this->region + ".amazonaws.com";
    QString canonicalHeaders = QString("host:%1\nx-amz-date:%2\n").arg(host).arg(paramAmzDate);
    QString payloadHash = QCryptographicHash::hash(QByteArray("", 0), QCryptographicHash::Sha256).toHex();

    QString canonicalRequest = method + "\n" + canonicalUri + "\n" + canonicalQueryString + "\n" + canonicalHeaders + "\n" + paramAmzSignedHeaders + "\n" + payloadHash;

    QString dateStamp = now.toString("yyyyMMdd");
    QString credentialScope = dateStamp + "/" + this->region + "/ec2/aws4_request";
    QString stringToSign = paramAmzAlgorithm + '\n' +  paramAmzDate + '\n' +  credentialScope + '\n' + QCryptographicHash::hash(canonicalRequest.toUtf8(), QCryptographicHash::Sha256).toHex();

    QByteArray signingKey = this->getSignatureKey(this->secretKey.toUtf8(), dateStamp.toUtf8());
    QString signature = QMessageAuthenticationCode::hash(stringToSign.toUtf8(), signingKey, QCryptographicHash::Sha256).toHex();

    // build HTTP request
    QString authorizationHeader = paramAmzAlgorithm + " Credential=" + this->accessKey + '/' + credentialScope + ", " +  "SignedHeaders=" + paramAmzSignedHeaders + ", " + "Signature=" + signature;

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://") + host + "/?" + canonicalQueryString));
    req.setRawHeader(QByteArray("Authorization"), authorizationHeader.toUtf8());
    req.setRawHeader(QByteArray("x-amz-date"), paramAmzDate.toUtf8());
    networkManager.get(req);
}

void AWSConnector::replyFinished(QNetworkReply *reply)
{
    AWSResult *result = new AWSResult();

    result->httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    result->httpBody = QString(reply->readAll());

    if (reply->error()) {
        result->isSuccess = false;
        result->errorString = reply->errorString();
    } else {
        result->isSuccess = true;
    }

    reply->deleteLater();
    reply = nullptr;

    emit awsReplyReceived(result);
}
