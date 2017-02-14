/*****************************************************************************
 *
 * This module contains functions for parsing the XML responses returned by
 * AWS API requests.
 *
 * The API reference with descriptions of the XML messages can be found here:
 *
 * http://docs.aws.amazon.com/AWSEC2/latest/APIReference/Welcome.html
 *
 ****************************************************************************/

#ifndef AWSRESPONSEPARSERS_H
#define AWSRESPONSEPARSERS_H

#include <memory>

#include <QBuffer>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "AWSConnector.h"
#include "AWSImage.h"
#include "AWSRouteTable.h"
#include "AWSSecurityGroup.h"
#include "AWSSubnet.h"
#include "AWSVpc.h"

std::vector<std::shared_ptr<AWSInstance>> parseDescribeInstancesResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSSecurityGroup>> parseDescribeSecurityGroupsResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSSubnet>> parseDescribeSubnetsResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSVpc>> parseDescribeVpcsResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSImage>> parseDescribeImagesResponse(AWSResult *result, QString region);
std::vector<std::shared_ptr<AWSRouteTable>> parseDescribeRouteTablesResponse(AWSResult *result, QString region);

#endif
