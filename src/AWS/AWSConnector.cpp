#include "AWSConnector.h"

const QStringList AWSConnector::Regions = QStringList()
    << "us-east-1" << "us-east-2" << "us-west-1" << "us-west-2" << "ca-central-1"
    << "eu-west-1" << "eu-west-2" << "eu-central-1" << "ap-northeast-1" << "ap-northeast-2"
    << "ap-southeast-1" << "ap-southeast-2" << "ap-south-1" << "sa-east-1";

const QStringList AWSConnector::RegionNames = QStringList()
    << "US East (N. Virginia)" << "US East (Ohio)" << "US West (N. California)" << "US West (Oregon)" << "Canada (Central)"
    << "EU (Ireland)" << "EU (London)" << "EU (Frankfurt)" << "Asia Pacific (Tokyo)" << "Asia Pacific (Seoul)"
    << "Asia Pacific (Singapore)" << "Asia Pacific (Sydney)" << "Asia Pacific (Mumbai)" << "South America (São Paulo)";

const QString AWSConnector::LOCATION_US_EAST_1 = "us-east-1";
const QString AWSConnector::LOCATION_US_EAST_2 = "us-east-2";

const QString AWSConnector::LOCATION_EU_WEST_1 = "eu-west-1";
const QString AWSConnector::LOCATION_EU_CENTRAL_1 = "eu-central-1";

AWSConnector::AWSConnector()
{
    connect(&this->networkManager, &QNetworkAccessManager::finished,
            this, &AWSConnector::replyFinished);
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

void AWSConnector::sendRequest(const QString action, const QString userType)
{
    QList<QString> extraParams;
    this->sendRequest(action, extraParams);
}

void AWSConnector::sendRequest(const QString action, QList<QString> &extraParams, const QString userType)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    QList<QString> params = extraParams;

    // Request Params
    QString paramAction = action;
    QString paramVersion = "2016-11-15";
    QString paramAmzAlgorithm = "AWS4-HMAC-SHA256";
    QString paramAmzDate = now.toString("yyyyMMddTHHmmssZ");
    QString paramAmzSignedHeaders = "host;x-amz-date";

    QString method = "GET";
    QString canonicalUri = "/";
    QByteArray canonicalQueryString;

    params.append(QString("Action=") + QUrl::toPercentEncoding(paramAction));
    params.append(QString("Version=") + QUrl::toPercentEncoding(paramVersion));
    std::sort(params.begin(), params.end());

    for (int i = 0; i < params.count(); i++) {
        if (i != 0) {
            canonicalQueryString += "&";
        }
        canonicalQueryString += params.at(i).toUtf8();
    }

    QString host = "ec2." + this->region + ".amazonaws.com";
    QString canonicalHeaders = QString("host:%1\nx-amz-date:%2\n").arg(host).arg(paramAmzDate);
    QString payloadHash = QCryptographicHash::hash(QByteArray("", 0), QCryptographicHash::Sha256).toHex();

    QString canonicalRequest = method + "\n" + canonicalUri + "\n" +
        canonicalQueryString + "\n" + canonicalHeaders + "\n" +
        paramAmzSignedHeaders + "\n" + payloadHash;

    QString dateStamp = now.toString("yyyyMMdd");
    QString credentialScope = dateStamp + "/" + this->region + "/ec2/aws4_request";
    QString stringToSign = paramAmzAlgorithm + '\n' +  paramAmzDate + '\n' +
        credentialScope + '\n' +
        QCryptographicHash::hash(canonicalRequest.toUtf8(), QCryptographicHash::Sha256).toHex();

    QByteArray signingKey = this->getSignatureKey(this->secretKey.toUtf8(), dateStamp.toUtf8());
    QString signature = QMessageAuthenticationCode::hash(stringToSign.toUtf8(), signingKey,
            QCryptographicHash::Sha256).toHex();

    // build HTTP request
    QString authorizationHeader = paramAmzAlgorithm + " Credential=" +
        this->accessKey + '/' + credentialScope + ", " +  "SignedHeaders=" +
        paramAmzSignedHeaders + ", " + "Signature=" + signature;

    QString urlStr = "https://" + host + "/?" + canonicalQueryString;

    QNetworkRequest req = QNetworkRequest(QUrl(urlStr));
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    sslConfiguration.setProtocol(QSsl::TlsV1_2);
    req.setSslConfiguration(sslConfiguration);
    req.setRawHeader(QByteArray("Authorization"), authorizationHeader.toUtf8());
    req.setRawHeader(QByteArray("x-amz-date"), paramAmzDate.toUtf8());
    QNetworkReply *reply = networkManager.get(req);
    reply->setProperty("userType", QVariant(userType));
}

void AWSConnector::describeInstances(const QString userType)
{
    QList<QString> extraParams;
    this->sendRequest("DescribeInstances", extraParams, userType);
}

void AWSConnector::describeInstances(QList<QString> &instanceIds, const QString userType)
{
    QList<QString> extraParams;
    for (int i = 0; i < instanceIds.count(); i++) {
        QString instanceIdParam = "InstanceId." + QString::number(i + 1) + "=" + instanceIds.at(i);
        extraParams.append(instanceIdParam);
    }

    this->sendRequest("DescribeInstances", extraParams, userType);
}

void AWSConnector::describeSecurityGroups(QList<QString> &groupIds, const QString userType)
{
    QList<QString> extraParams;
    for (int i = 0; i < groupIds.count(); i++) {
        QString groupIdParam = "GroupId." + QString::number(i + 1) + "=" + groupIds.at(i);
        extraParams.append(groupIdParam);
    }

    this->sendRequest("DescribeSecurityGroups", extraParams, userType);
}

void AWSConnector::describeSubnets(QList<QString> &subnetIds, const QString userType)
{
    QList<QString> extraParams;
    for (int i = 0; i < subnetIds.count(); i++) {
        QString subnetIdParam = "SubnetId." + QString::number(i + 1) + "=" + subnetIds.at(i);
        extraParams.append(subnetIdParam);
    }

    this->sendRequest("DescribeSubnets", extraParams, userType);
}

void AWSConnector::describeVpcs(const QString userType)
{
    QList<QString> vpcIds;
    this->sendRequest("DescribeVpcs", vpcIds, userType);
}

void AWSConnector::describeVpcs(QList<QString> &vpcIds, const QString userType)
{
    QList<QString> extraParams;
    for (int i = 0; i < vpcIds.count(); i++) {
        QString vpcIdParam = "VpcId." + QString::number(i + 1) + "=" + vpcIds.at(i);
        extraParams.append(vpcIdParam);
    }

    this->sendRequest("DescribeVpcs", extraParams, userType);
}

void AWSConnector::describeImages(QList<QString> &imageIds, const QString userType)
{
    QList<QString> extraParams;
    for (int i = 0; i < imageIds.count(); i++) {
        QString imageIdParam = "ImageId." + QString::number(i + 1) + "=" + imageIds.at(i);
        extraParams.append(imageIdParam);
    }

    this->sendRequest("DescribeImages", extraParams, userType);
}

void AWSConnector::describeRouteTables(const QString userType)
{
    QList<QString> extraParams;

    this->sendRequest("DescribeRouteTables", extraParams, userType);
}

void AWSConnector::describeRouteTablesWithSubnetId(QString &subnetId, const QString userType)
{
    QList<QString> extraParams;
    QString filterName = "Filter.1.Name=association.subnet-id";
    QString filterValue = "Filter.1.Value.1=" + subnetId;
    extraParams.append(filterName);
    extraParams.append(filterValue);

    this->sendRequest("DescribeRouteTables", extraParams, userType);
}

void AWSConnector::describeRouteTableMain(QString &vpcId, const QString userType)
{
    QList<QString> extraParams;
    QString filterName = "Filter.1.Name=association.main";
    QString filterValue = "Filter.1.Value.1=true";
    extraParams.append(filterName);
    extraParams.append(filterValue);

    filterName = "Filter.2.Name=vpc-id";
    filterValue = "Filter.2.Value=" + vpcId;
    extraParams.append(filterName);
    extraParams.append(filterValue);

    this->sendRequest("DescribeRouteTables", extraParams, userType);
}

void AWSConnector::replyFinished(QNetworkReply *reply)
{
    AWSResult *result = new AWSResult();

    result->httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    result->httpBody = QString(reply->readAll());

    // find the response type (the name of the enclosing XML tag)
    QRegularExpression re("<([a-zA-Z]+)");
    QRegularExpressionMatch match = re.match(result->httpBody);
    if (match.hasMatch()) {
        result->responseType = match.captured(1);
    }

    if (reply->error()) {
        result->isSuccess = false;
        result->errorString = reply->errorString();
    } else {
        result->isSuccess = true;
    }

    result->userType = reply->property("userType").toString();

    reply->deleteLater();
    reply = nullptr;

    emit awsReplyReceived(result);
}

void AWSConnector::stopInstances(QList<QString> instanceIds)
{
    QList<QString> extraParams;

    int i = 0;
    for (QString instanceId : instanceIds) {
        i++;
        extraParams.append("InstanceId." + QString::number(i) + "=" + instanceId);
    }

    this->sendRequest("StopInstances", extraParams);
}

void AWSConnector::startInstances(QList<QString> instanceIds)
{
    QList<QString> extraParams;

    int i = 0;
    for (QString instanceId : instanceIds) {
        i++;
        extraParams.append("InstanceId." + QString::number(i) + "=" + instanceId);
    }

    this->sendRequest("StartInstances", extraParams);
}

void AWSConnector::rebootInstances(QList<QString> instanceIds)
{
    QList<QString> extraParams;

    int i = 0;
    for (QString instanceId : instanceIds) {
        i++;
        extraParams.append("InstanceId." + QString::number(i) + "=" + instanceId);
    }

    this->sendRequest("RebootInstances", extraParams);
}

void AWSConnector::terminateInstances(QList<QString> instanceIds)
{
    QList<QString> extraParams;

    int i = 0;
    for (QString instanceId : instanceIds) {
        i++;
        extraParams.append("InstanceId." + QString::number(i) + "=" + instanceId);
    }

    this->sendRequest("TerminateInstances", extraParams);
}
