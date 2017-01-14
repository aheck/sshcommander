#include "AWSResponseParsers.h"

static void parseAWSIngressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup);
static void parseAWSEgressPermissions(QXmlStreamReader &xml, std::shared_ptr<AWSSecurityGroup> securityGroup);
static void parseAWSTags(QXmlStreamReader &xml, QList<AWSTag> &tags);

std::vector<std::shared_ptr<AWSInstance>> parseDescribeInstancesResponse(AWSResult *result, QString region)
{
    std::vector<std::shared_ptr<AWSInstance>> vector;
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
                    vector.push_back(instance);
                } else if (groupSet && itemLevel == 3) {
                    AWSSecurityGroup securityGroup;
                    instance->securityGroups.append(securityGroup);
                }
            } else if (name == "instanceState") {
                instanceState = true;
            } else if (name == "groupSet") {
                groupSet = true;
            } else if (name == "tagSet") {
                parseAWSTags(xml, instance->tags);

                for (AWSTag tag : instance->tags) {
                    if (tag.key == "Name") {
                        instance->name = tag.value;
                    } else if (tag.key == "aws:cloudformation:stack-id") {
                        instance->cfStackId = tag.value;
                    } else if (tag.key == "aws:cloudformation:stack-name") {
                        instance->cfStackName = tag.value;
                    }
                }
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
                } else if (name == "sourceDestCheck") {
                    instance->sourceDestCheck = xml.readElementText();
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
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "instancesSet") {
                instancesSet = false;
            } else if (name == "groupSet") {
                groupSet = false;
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

std::vector<std::shared_ptr<AWSSecurityGroup>> parseDescribeSecurityGroupsResponse(AWSResult *result, QString region)
{
    std::vector<std::shared_ptr<AWSSecurityGroup>> vector;
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
                    vector.push_back(securityGroup);
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

static void parseAWSTags(QXmlStreamReader &xml, QList<AWSTag> &tags)
{
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();

            if (name == "item") {
                itemLevel++;

                if (itemLevel == 1) {
                    AWSTag tag;
                    tags.append(tag);
                }
                continue;
            }

            if (itemLevel == 1) {
                if (name == "key") {
                    tags.last().key = xml.readElementText();
                } else if (name == "value") {
                    tags.last().value = xml.readElementText();
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "tagSet") {
                break;
            } else if (name == "item") {
                itemLevel--;
            }
        }
    }
}

std::vector<std::shared_ptr<AWSSubnet>> parseDescribeSubnetsResponse(AWSResult *result, QString region)
{
    std::vector<std::shared_ptr<AWSSubnet>> vector;
    std::shared_ptr<AWSSubnet> subnet;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool subnetSet = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "subnetSet") {
                subnetSet = true;
            } else if (name == "item") {
                itemLevel++;

                // new subnet item?
                if (itemLevel == 1) {
                    subnet = std::make_shared<AWSSubnet>();
                    vector.push_back(subnet);
                }
            } else if (subnetSet && itemLevel == 1) {
                if (name == "subnetId") {
                    subnet->id = xml.readElementText();
                } else if (name == "state") {
                    subnet->state = xml.readElementText();
                } else if (name == "vpcId") {
                    subnet->vpcId = xml.readElementText();
                } else if (name == "cidrBlock") {
                    subnet->cidrBlock = xml.readElementText();
                } else if (name == "availableIpAddressCount") {
                    subnet->availableIpAddressCount = xml.readElementText();
                } else if (name == "availabilityZone") {
                    subnet->availabilityZone = xml.readElementText();
                } else if (name == "defaultForAz") {
                    subnet->defaultForAz = xml.readElementText() == "true";
                } else if (name == "mapPublicIpOnLaunch") {
                    subnet->mapPublicIpOnLaunch = xml.readElementText() == "true";
                } else if (name == "tagSet") {
                    parseAWSTags(xml, subnet->tags);

                    for (AWSTag tag : subnet->tags) {
                        if (tag.key == "Name") {
                            subnet->name = tag.value;
                        }
                    }
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "subnetSet") {
                subnetSet = false;
            } else if (name == "item") {
                itemLevel--;
            }
        }
    }

    return vector;
}

std::vector<std::shared_ptr<AWSVpc>> parseDescribeVpcsResponse(AWSResult *result, QString region)
{
    std::vector<std::shared_ptr<AWSVpc>> vector;
    std::shared_ptr<AWSVpc> vpc;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool vpcSet = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "vpcSet") {
                vpcSet = true;
            } else if (name == "item") {
                itemLevel++;

                // new vpc item?
                if (itemLevel == 1) {
                    vpc = std::make_shared<AWSVpc>();
                    vector.push_back(vpc);
                }
            } else if (vpcSet && itemLevel == 1) {
                if (name == "vpcId") {
                    vpc->id = xml.readElementText();
                } else if (name == "state") {
                    vpc->state = xml.readElementText();
                } else if (name == "cidrBlock") {
                    vpc->cidrBlock = xml.readElementText();
                } else if (name == "dhcpOptionsId") {
                    vpc->dhcpOptionsId = xml.readElementText();
                } else if (name == "instanceTenancy") {
                    vpc->instanceTenancy = xml.readElementText();
                } else if (name == "isDefault") {
                    vpc->isDefault = xml.readElementText() == "true";
                } else if (name == "tagSet") {
                    parseAWSTags(xml, vpc->tags);

                    for (AWSTag tag : vpc->tags) {
                        if (tag.key == "Name") {
                            vpc->name = tag.value;
                        }
                    }
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "vpcSet") {
                vpcSet = false;
            } else if (name == "item") {
                itemLevel--;
            }
        }
    }

    return vector;
}

std::vector<std::shared_ptr<AWSImage>> parseDescribeImagesResponse(AWSResult *result, QString region)
{
    std::vector<std::shared_ptr<AWSImage>> vector;
    std::shared_ptr<AWSImage> image;

    if (result->httpBody.isEmpty()) {
        return vector;
    }

    QBuffer buffer;
    buffer.setData(result->httpBody.toUtf8());
    buffer.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool imagesSet = false;
    int itemLevel = 0;

    while (!xml.isEndDocument()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString name = xml.name().toString();
            if (name == "imagesSet") {
                imagesSet = true;
            } else if (name == "item") {
                itemLevel++;

                // new image item?
                if (itemLevel == 1) {
                    image = std::make_shared<AWSImage>();
                    vector.push_back(image);
                }
            } else if (imagesSet && itemLevel == 1) {
                if (name == "imageId") {
                    image->id = xml.readElementText();
                } else if (name == "imageLocation") {
                    image->location = xml.readElementText();
                } else if (name == "imageState") {
                    image->state = xml.readElementText();
                } else if (name == "imageOwnerId") {
                    image->ownerId = xml.readElementText();
                } else if (name == "isPublic") {
                    image->isPublic = xml.readElementText() == "true";
                } else if (name == "architecture") {
                    image->architecture = xml.readElementText();
                } else if (name == "imageType") {
                    image->type = xml.readElementText();
                } else if (name == "kernelId") {
                    image->kernelId = xml.readElementText();
                } else if (name == "ramdiskId") {
                    image->ramdiskId = xml.readElementText();
                } else if (name == "imageOwnerAlias") {
                    image->ownerAlias = xml.readElementText();
                } else if (name == "name") {
                    image->name = xml.readElementText();
                } else if (name == "description") {
                    image->description = xml.readElementText();
                } else if (name == "rootDeviceType") {
                    image->rootDeviceType = xml.readElementText();
                } else if (name == "rootDeviceName") {
                    image->rootDeviceName = xml.readElementText();
                } else if (name == "virtualizationType") {
                    image->virtualizationType = xml.readElementText();
                } else if (name == "hypervisor") {
                    image->hypervisor = xml.readElementText();
                } else if (name == "creationDate") {
                    image->creationDate = xml.readElementText();
                } else if (name == "tagSet") {
                    parseAWSTags(xml, image->tags);
                }
            }
        } else if (xml.isEndElement()) {
            QString name = xml.name().toString();
            if (name == "imagesSet") {
                imagesSet = false;
            } else if (name == "item") {
                itemLevel--;
            }
        }
    }

    return vector;
}
