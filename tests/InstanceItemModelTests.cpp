#include "InstanceItemModelTests.h"

void InstanceItemModelTests::testAddInstances()
{
    std::vector<std::shared_ptr<AWSInstance>> instances;
    InstanceItemModel model;
    std::shared_ptr<AWSInstance> instance;

    // add three instances to the model
    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 1";
    instances.push_back(instance);

    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 2";
    instances.push_back(instance);

    instance = std::make_shared<AWSInstance>();
    instance->name = "Instance 3";
    instances.push_back(instance);

    model.setInstances(instances);

    // the model should have three rows now
    QVERIFY(model.rowCount(QModelIndex()) == 3);

    // check if the model contains the instances we put in
    QVERIFY(model.getInstance(model.index(0, 0, QModelIndex()))->name == "Instance 1");
    QVERIFY(model.getInstance(model.index(1, 0, QModelIndex()))->name == "Instance 2");
    QVERIFY(model.getInstance(model.index(2, 0, QModelIndex()))->name == "Instance 3");
}

void InstanceItemModelTests::testSortingManyInstances()
{
    //auto instances = std::make_unique<QVector<std::shared_ptr<AWSInstance>>>();
    std::vector<std::shared_ptr<AWSInstance>> instances;
    InstanceItemModel model;
    std::shared_ptr<AWSInstance> instance;

    for (int i = 0; i < 1000; i++) {
        instance = std::make_shared<AWSInstance>();
        instance->name = "Instance " + QString::number(i);
        instances.push_back(instance);
    }

    model.setInstances(instances);

    // the model should have 1000 rows now
    QVERIFY(model.rowCount(QModelIndex()) == 1000);
    auto parent = QModelIndex();

    for (int i = 0; i < 1000; i++) {
        QVERIFY(model.getInstance(model.index(i, 0, parent))->name == "Instance " + QString::number(i));
    }

    model.sort(1, Qt::DescendingOrder);

    QVERIFY(model.getInstance(model.index(0, 0, parent))->name == "Instance 999");
    QVERIFY(model.getInstance(model.index(999, 0, parent))->name == "Instance 0");

    model.sort(1, Qt::AscendingOrder);

    QVERIFY(model.getInstance(model.index(0, 0, parent))->name == "Instance 0");
    QVERIFY(model.getInstance(model.index(999, 0, parent))->name == "Instance 999");
}

void InstanceItemModelTests::testComparatorOrdering()
{
    InstanceComparator cmp;
    std::shared_ptr<AWSInstance> a = std::make_shared<AWSInstance>();
    std::shared_ptr<AWSInstance> a2 = std::make_shared<AWSInstance>();
    std::shared_ptr<AWSInstance> b = std::make_shared<AWSInstance>();

    a->name = "Instance A";
    a2->name = "Instance A";
    b->name = "Instance B";

    cmp.column = 1;

    QVERIFY(cmp(a, a) == false);
    QVERIFY(cmp(a, a2) == false);
    QVERIFY(cmp(a2, a) == false);

    QVERIFY(cmp(a, b) == true);
    QVERIFY(cmp(b, a) == false);
}
