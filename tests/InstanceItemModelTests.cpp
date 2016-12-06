#include "InstanceItemModelTests.h"

void InstanceItemModelTests::testAddInstances()
{
    QVector<std::shared_ptr<AWSInstance>> instances;
    InstanceItemModel model;
    std::shared_ptr<AWSInstance> instance;

    // add three instances to the model
    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 1";
    instances.append(instance);

    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 2";
    instances.append(instance);

    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 3";
    instances.append(instance);

    model.setInstances(instances);

    // the model should have three rows now
    QVERIFY(model.rowCount(QModelIndex()) == 3);

    // check if the model contains the instances we put in
    QVERIFY(model.getInstance(model.index(0, 0, QModelIndex()))->name == "Instance 1");
    QVERIFY(model.getInstance(model.index(1, 0, QModelIndex()))->name == "Instance 2");
    QVERIFY(model.getInstance(model.index(2, 0, QModelIndex()))->name == "Instance 3");
}
