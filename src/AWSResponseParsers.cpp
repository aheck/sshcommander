#include "AWSResponseParsers.h"

static void parseAWSIngressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup);
static void parseAWSEgressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup);

QVector<std::shared_ptr<AWSInstance>> parseDescribeInstancesResponse(AWSResult *result, QString region)
{
    QVector<std::shared_ptr<AWSInstance>> vector;
    std::shared_ptr<AWSInstance> instance;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool instancesSet = false;
    bool instanceState = false;
    bool groupSet = false;
    bool tagSet = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "instancesSet") {
                instancesSet = true;
            } else if (name == "item") {
                itemLevel++;

                if (instancesSet && itemLevel == 2) {
                    instance = std::make_shared<AWSInstance>();
                    instance->region = region;
                    vector << instance;
                } else if (groupSet && itemLevel == 3) {
                    AWSSecurityGroup securityGroup;
                    instance->securityGroups.append(securityGroup);
                } else if (tagSet && itemLevel == 3) {
                    AWSTag tag;
                    instance->tags.append(tag);
                }
            } else if (name == "instanceState") {
                instanceState = true;
            } else if (name == "groupSet"){
                groupSet = true;
            } else if (name == "tagSet"){
                tagSet = true;
            } else if (instancesSet && itemLevel == 2) {
                if (name == "instanceId") {
                    instance->id = xml.readElementText();
                } else if (name == "name" && instanceState == true) {
                    instance->status = xml.readElementText();
                } else if (name == "instanceType") {
                    instance->type = xml.readElementText();
                } else if (name == "keyName") {
                    instance->keyname = xml.readElementText();
                } else if (name == "availabilityZone") {
                    instance->availabilityZone = xml.readElementText();
                } else if (name == "imageId") {
                    instance->imageId = xml.readElementText();
                } else if (name == "ipAddress") {
                    instance->publicIP = xml.readElementText();
                } else if (name == "privateIpAddress") {
                    instance->privateIP = xml.readElementText();
                } else if (name == "launchTime") {
                    instance->launchTime = xml.readElementText();
                } else if (name == "vpcId") {
                    instance->vpcId = xml.readElementText();
                } else if (name == "subnetId") {
                    instance->subnetId = xml.readElementText();
                } else if (name == "virtualizationType") {
                    instance->virtualizationType = xml.readElementText();
                } else if (name == "hypervisor") {
                    instance->hypervisor = xml.readElementText();
                } else if (name == "architecture") {
                    instance->architecture = xml.readElementText();
                }
            } else if (groupSet && itemLevel == 3) {
                if (name == "groupId") {
                    instance->securityGroups.last().id = xml.readElementText();
                } else if (name == "groupName") {
                    instance->securityGroups.last().name = xml.readElementText();
                }
            } else if (tagSet && itemLevel == 3) {
                if (name == "key") {
                    instance->tags.last().key = xml.readElementText();
                    if (instance->tags.last().key == "Name") {
                        instance->name = instance->tags.last().value;
                    }
                } else if (name == "value") {
                    instance->tags.last().value = xml.readElementText();
                    if (instance->tags.last().key == "Name") {
                        instance->name = instance->tags.last().value;
                    }
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "instancesSet") {
                instancesSet = false;
            } else if (name == "groupSet") {
                groupSet = false;
            } else if (name == "tagSet") {
                tagSet = false;
            } else if (name == "item") {
                itemLevel--;
            } else if (name == "instanceState") {
                instanceState = false;
            }

            if (instancesSet && itemLevel < 2) {
                instance = nullptr;
            }
        }
    }

    if (xml.hasError()) {
        std::cout << "XML error: " << xml.errorString().data() << std::endl;
    }

    return vector;
}

QVector<std::shared_ptr<AWSSecurityGroup>> parseDescribeSecurityGroupsResponse(AWSResult *result, QString region)
{
    QVector<std::shared_ptr<AWSSecurityGroup>> vector;
    std::shared_ptr<AWSSecurityGroup> securityGroup;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool securityGroupInfo = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "securityGroupInfo") {
                securityGroupInfo = true;
            } else if (name == "item") {
                itemLevel++;

                // new security group item?
                if (itemLevel == 1) {
                    securityGroup = std::make_shared<AWSSecurityGroup>();
                    vector.append(securityGroup);
                }
            } else if (securityGroupInfo) {
                if (name == "groupId") {
                    securityGroup->id = xml.readElementText();
                } else if (name == "groupName") {
                    securityGroup->name = xml.readElementText();
                } else if (name == "groupDescription") {
                    securityGroup->description = xml.readElementText();
                } else if (name == "vpcId") {
                    securityGroup->vpcId = xml.readElementText();
                } else if (name == "ipPermissions") {
                    parseAWSIngressPermissions(xml, securityGroup);
                } else if (name == "ipPermissionsEgress") {
                    parseAWSEgressPermissions(xml, securityGroup);
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "securityGroupInfo") {
                securityGroupInfo = false;
            } else if (name == "item") {
                itemLevel--;
            }
        }
    }

    return vector;
}

static void parseAWSIngressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup)
{
    bool ipRanges = false;
    int itemLevel = 0;

    AWSIngressPermission perm;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "item") {
                itemLevel++;

                if (itemLevel == 1) {
                    perm = AWSIngressPermission();
                }
                continue;
            }

            if (name == "ipProtocol") {
                perm.ipProtocol = xml.readElementText();
            } else if (name == "fromPort") {
                perm.fromPort = xml.readElementText();
            } else if (name == "toPort") {
                perm.toPort = xml.readElementText();
            } else if (name == "ipRanges") {
                ipRanges = true;
            } else if (name == "cidrIp" && itemLevel == 2 && ipRanges) {
                perm.cidrs.append(xml.readElementText());
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "ipPermissions") {
                break;
            } else if (name == "item") {
                if (itemLevel == 1) {
                    securityGroup->ingressPermissions.append(perm);
                }
                itemLevel--;
            } else if (name == "ipRanges") {
                ipRanges = false;
            }
        }
    }
}

static void parseAWSEgressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup)
{
    bool ipRanges = false;
    int itemLevel = 0;

    AWSEgressPermission perm;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "item") {
                itemLevel++;

                if (itemLevel == 1) {
                    perm = AWSEgressPermission();
                }
                continue;
            }

            if (name == "ipProtocol") {
                perm.ipProtocol = xml.readElementText();
            } else if (name == "fromPort") {
                perm.fromPort = xml.readElementText();
            } else if (name == "toPort") {
                perm.toPort = xml.readElementText();
            } else if (name == "ipRanges") {
                ipRanges = true;
            } else if (name == "cidrIp" && itemLevel == 2 && ipRanges) {
                perm.cidrs.append(xml.readElementText());
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "ipPermissionsEgress") {
                break;
            } else if (name == "item") {
                if (itemLevel == 1) {
                    securityGroup->egressPermissions.append(perm);
                }
                itemLevel--;
            } else if (name == "ipRanges") {
                ipRanges = false;
            }
        }
    }
}