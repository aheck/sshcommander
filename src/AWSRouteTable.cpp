#include "AWSRouteTable.h"

AWSRouteTable::AWSRouteTable()
{

}

void AWSRouteTable::read(const QJsonObject &json)
{
    this->id = json["id"].toString();
    this->vpcId = json["vpcId"].toString();
    this->name = json["name"].toString();

    for (QJsonValueRef ref: json["routes"].toArray()) {
        AWSRoute route;
        route.read(ref.toObject());
        this->routes.append(route);
    }
}

void AWSRouteTable::write(QJsonObject &json) const
{
    json["id"] = this->id;
    json["vpcId"] = this->vpcId;
    json["name"] = this->name;

    QJsonArray routesArray;
    for (AWSRoute route: this->routes) {
        QJsonObject obj;
        route.write(obj);
        routesArray.append(obj);
    }
    json["routes"] = routesArray;
}
