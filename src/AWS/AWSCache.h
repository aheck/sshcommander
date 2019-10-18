/*****************************************************************************
 *
 * AWSCache is a singleton for caching AWS objects that are created by
 * querying the AWS API.
 *
 * When you query objects in the AWS API they often refer to other objects
 * that are not retrieved in the same call. Therefore AWS objects queried in
 * bulk requests as well as single object updates can be cached in with this
 * singleton to allow the application to make sense of references without
 * recursively calling the AWS API all the time.
 *
 * AWS objects are keyed by the following scheme: "{REGION}#{AWS-ID}"
 * E.g.: "us-east-1#i-123456"
 *
 ****************************************************************************/

#ifndef AWSCACHE_H
#define AWSCACHE_H

#include <map>
#include <memory>
#include <vector>

#include <QString>

#include "AWSInstance.h"
#include "AWSRouteTable.h"
#include "AWSSecurityGroup.h"
#include "AWSSubnet.h"
#include "AWSVpc.h"

class AWSInstance;

class AWSCache
{
public:
    static AWSCache& getInstance();

    AWSCache(AWSCache const &other) = delete;
    void operator=(AWSCache const &) = delete;

    void clearInstances(QString region);
    void clearSecurityGroups(QString region);
    void clearSubnets(QString region);
    void clearVpcs(QString region);
    void clearRouteTables(QString region);

    void updateInstances(const QString region, std::vector<std::shared_ptr<AWSInstance>> &instances);
    void updateSecurityGroups(const QString region, std::vector<std::shared_ptr<AWSSecurityGroup>> &securityGroups);
    void updateSubnets(const QString region, std::vector<std::shared_ptr<AWSSubnet>> &subnets);
    void updateVpcs(const QString region, std::vector<std::shared_ptr<AWSVpc>> &vpcs);
    void updateRouteTables(const QString region, std::vector<std::shared_ptr<AWSRouteTable>> &routeTables);

    std::shared_ptr<AWSInstance> resolveInstance(const QString region, const QString instanceId);
    std::shared_ptr<AWSSecurityGroup> resolveSecurityGroup(const QString region, const QString instanceId);
    std::shared_ptr<AWSSubnet> resolveSubnet(const QString region, const QString subnetId);
    std::shared_ptr<AWSVpc> resolveVpc(const QString region, const QString vpcId);
    std::shared_ptr<AWSRouteTable> resolveRouteTable(const QString region, const QString routeTableId);

private:
    AWSCache();
    ~AWSCache();

    std::map<const QString, std::shared_ptr<AWSInstance>> instanceCache;
    std::map<const QString, std::shared_ptr<AWSSecurityGroup>> securityGroupCache;
    std::map<const QString, std::shared_ptr<AWSSubnet>> subnetCache;
    std::map<const QString, std::shared_ptr<AWSVpc>> vpcCache;
    std::map<const QString, std::shared_ptr<AWSRouteTable>> routeTableCache;

    const QString buildKey(const QString region, const QString id);
};

#endif
