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

#include <QString>

#include "AWSInstance.h"
#include "AWSSecurityGroup.h"

class AWSCache
{
public:
    static AWSCache& getInstance();

    void updateInstances(const QString region, std::vector<std::shared_ptr<AWSInstance>> instances);
    void updateSecurityGroups(const QString region, std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups);

    std::shared_ptr<AWSInstance> resolveInstance(const QString region, const QString instanceId);
    std::shared_ptr<AWSSecurityGroup> resolveSecurityGroup(const QString region, const QString instanceId);

private:
    AWSCache();
    ~AWSCache();

    std::map<const QString, std::shared_ptr<AWSInstance>> instanceCache;
    std::map<const QString, std::shared_ptr<AWSSecurityGroup>> securityGroupCache;

    const QString buildKey(const QString region, const QString id);
};

#endif
