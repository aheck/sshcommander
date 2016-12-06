#include <QObject>
#include <QVector>

#include <QtTest/QtTest>

#include <memory>

#include "AWSInstance.h"
#include "InstanceItemModel.h"

class InstanceItemModelTests : public QObject
{
    Q_OBJECT
private slots:
    void testAddInstances();
};
