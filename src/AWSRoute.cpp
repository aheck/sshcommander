#include "AWSRoute.h"

AWSRoute::AWSRoute()
{

}

void AWSRoute::read(const QJsonObject &json)
{
    this->destinationCidrBlock = json["destinationCidrBlock"].toString();
    this->destinationIpv6CidrBlock = json["destinationIpv6CidrBlock"].toString();
    this->gatewayId = json["gatewayId"].toString();
    this->state = json["state"].toString();
    this->origin = json["origin"].toString();
}

void AWSRoute::write(QJsonObject &json) const
{
    json["destinationCidrBlock"] = this->destinationCidrBlock;
    json["destinationIpv6CidrBlock"] = this->destinationIpv6CidrBlock;
    json["gatewayId"] = this->gatewayId;
    json["state"] = this->state;
    json["origin"] = this->origin;
}
