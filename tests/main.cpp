#include <QtTest/QtTest>

#include "InstanceItemModelTests.h"
#include "PseudoTerminalTests.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    InstanceItemModelTests instanceItemModelTests;
    PseudoTerminalTests pseudoTerminalTests;

    QTest::qExec(&instanceItemModelTests);
    QTest::qExec(&pseudoTerminalTests, argc, argv);

    return 0;
}
