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
    this->subnetCache.clear();
    this->vpcCache.clear();
}

void AWSCache::clearInstances(QString region)
{
    auto iter = this->instanceCache.begin();
    while (iter != this->instanceCache.end()) {
        if (iter->first.startsWith(region)) {
            this->instanceCache.erase(iter++);
        } else {
            ++iter;
        }
    }
}

void AWSCache::clearSecurityGroups(QString region)
{
    auto iter = this->securityGroupCache.begin();
    while (iter != this->securityGroupCache.end()) {
        if (iter->first.startsWith(region)) {
            this->securityGroupCache.erase(iter++);
        } else {
            ++iter;
        }
    }
}

void AWSCache::clearSubnets(QString region)
{
    auto iter = this->subnetCache.begin();
    while (iter != this->subnetCache.end()) {
        if (iter->first.startsWith(region)) {
            this->subnetCache.erase(iter++);
        } else {
            ++iter;
        }
    }
}

void AWSCache::clearVpcs(QString region)
{
    auto iter = this->vpcCache.begin();
    while (iter != this->vpcCache.end()) {
        if (iter->first.startsWith(region)) {
            this->vpcCache.erase(iter++);
        } else {
            ++iter;
        }
    }
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

void AWSCache::updateSubnets(const QString region, std::vector<std::shared_ptr<AWSSubnet>> subnets)
{
    for (auto subnet : subnets) {
        QString key = buildKey(region, subnet->id);
        this->subnetCache[key] = subnet;
    }
}

void AWSCache::updateVpcs(const QString region, std::vector<std::shared_ptr<AWSVpc>> vpcs)
{
    for (auto vpc : vpcs) {
        QString key = buildKey(region, vpc->id);
        this->vpcCache[key] = vpc;
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

std::shared_ptr<AWSSubnet> AWSCache::resolveSubnet(const QString region, const QString subnetId)
{
    QString key = buildKey(region, subnetId);
    return this->subnetCache[key];
}

std::shared_ptr<AWSVpc> AWSCache::resolveVpc(const QString region, const QString vpcId)
{
    QString key = buildKey(region, vpcId);
    return this->vpcCache[key];
}

inline const QString AWSCache::buildKey(const QString region, const QString id)
{
    QString key = region + "#" + id;
    return key;
}
