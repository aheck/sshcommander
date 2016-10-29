#include "AWSConnector.h"

const QString AWSConnector::LOCATION_US_EAST_1 = "us-east-1";
const QString AWSConnector::LOCATION_US_EAST_2 = "us-east-2";

const QString AWSConnector::LOCATION_EU_WEST_1 = "eu-west-1";
const QString AWSConnector::LOCATION_EU_CENTRAL_1 = "eu-central-1";

const QString EC2_DEFAULT_ENDPOINT = "ec2.amazonaws.com";

AWSConnector::AWSConnector()
{
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

AWSResult* AWSConnector::describeInstances()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    AWSResult *result = new AWSResult();

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

    QString host = "ec2.amazonaws.com";
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

    result->isSuccess = true;
    result->httpStatus = 200;
    result->replyBody = QString("");

    connect(&this->networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://") + EC2_DEFAULT_ENDPOINT + "/?" + canonicalQueryString));
    req.setRawHeader(QByteArray("Authorization"), authorizationHeader.toUtf8());
    req.setRawHeader(QByteArray("x-amz-date"), paramAmzDate.toUtf8());
    networkManager.get(req);

    return result;
}

void AWSConnector::replyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
        std::cout << reply->readAll().toStdString();
        std::cout << std::endl;
    } else {
        std::cout << "Content-Type: " << reply->header(QNetworkRequest::ContentTypeHeader).toString().toStdString() << std::endl;
        std::cout << "Content-Length: " << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong() << std::endl;
        std::cout << "Status Code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;

        std::cout << reply->readAll().toStdString() << std::endl;
    }

    reply->deleteLater();
}
