#ifndef AWSRESPONSEPARSERS_H
#define AWSRESPONSEPARSERS_H

#include <memory>

#include <QBuffer>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "AWSConnector.h"
#include "AWSSecurityGroup.h"

std::vector<std::shared_ptr<AWSInstance>> parseDescribeInstancesResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSSecurityGroup>> parseDescribeSecurityGroupsResponse(AWSResult *result, QString region);

#endif
