#include <QtTest/QtTest>

#include "InstanceItemModelTests.h"
#include "PseudoTerminalTests.h"
#include "KnownHostsTests.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    InstanceItemModelTests instanceItemModelTests;
    PseudoTerminalTests pseudoTerminalTests;
    KnownHostsTests knownHostsTests;

    QTest::qExec(&instanceItemModelTests);
    QTest::qExec(&pseudoTerminalTests, argc, argv);
    QTest::qExec(&knownHostsTests, argc, argv);

    return 0;
}
