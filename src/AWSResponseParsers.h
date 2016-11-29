#ifndef AWSRESPONSEPARSERS_H
#define AWSRESPONSEPARSERS_H

#include <memory>

#include <QBuffer>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QXmlStreamReader>

#include "AWSConnector.h"
#include "AWSSecurityGroup.h"

QVector<std::shared_ptr<AWSInstance>> parseDescribeInstancesResponse(AWSResult *result, QString region);
QVector<std::shared_ptr<AWSSecurityGroup>> parseDescribeSecurityGroupsResponse(AWSResult *result, QString region);

#endif
