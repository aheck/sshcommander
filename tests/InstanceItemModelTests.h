#include <QObject>

#include <QtTest/QtTest>

#include <memory>

#include "AWS/AWSInstance.h"
#include "InstanceItemModel.h"

class InstanceItemModelTests : public QObject
{
    Q_OBJECT
private slots:
    void testAddInstances();
    void testSortingManyInstances();
    void testComparatorOrdering();
};
