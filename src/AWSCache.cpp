#include "AWSCache.h"

AWSCache& AWSCache::getInstance()
{
    static AWSCache awsCache;
    return awsCache;
}

AWSCache::AWSCache()
{
}

AWSCache::~AWSCache()
{
    this->instanceCache.clear();
    this->securityGroupCache.clear();
}

void AWSCache::updateInstances(const QString region, std::vector<std::shared_ptr<AWSInstance>> instances)
{
    for (auto instance : instances) {
        QString key = buildKey(region, instance->id);
        this->instanceCache[key] = instance;
    }
}

void AWSCache::updateSecurityGroups(const QString region, std::vector<std::shared_ptr<AWSSecurityGroup>> securityGroups)
{
    for (auto securityGroup : securityGroups) {
        QString key = buildKey(region, securityGroup->id);
        this->securityGroupCache[key] = securityGroup;
    }
}

std::shared_ptr<AWSInstance> AWSCache::resolveInstance(const QString region, const QString instanceId)
{
    QString key = buildKey(region, instanceId);
    return this->instanceCache[key];
}

std::shared_ptr<AWSSecurityGroup> AWSCache::resolveSecurityGroup(const QString region, const QString securityGroupId)
{
    QString key = buildKey(region, securityGroupId);
    return this->securityGroupCache[key];
}

inline const QString AWSCache::buildKey(const QString region, const QString id)
{
    QString key = region + "#" + id;
    return key;
}
