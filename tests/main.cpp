#include <QtTest/QtTest>

#include "InstanceItemModelTests.h"

int main(int argc, char **argv)
{
    InstanceItemModelTests instanceItemModelTests;

    QTest::qExec(&instanceItemModelTests);

    return 0;
}
