#include "PseudoTerminalTests.h"

void PseudoTerminalTests::testBasicOperation()
{
    PseudoTerminal term;

    connect(&term, &PseudoTerminal::lineReceived, this, &PseudoTerminalTests::lineReceived);
    QSignalSpy dataSpy(&term, SIGNAL(lineReceived(QString)));
    QSignalSpy finishedSpy(&term, SIGNAL(finished(int)));

    term.start("/bin/bash");
    QTest::qWait(500);
    term.sendData("echo \"Hello World\"\n");
    QTest::qWait(500);

    QCOMPARE(finishedSpy.count(), 0);
    term.sendData("exit 5\n");
    QTest::qWait(500);

    QCOMPARE(finishedSpy.count(), 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 5);
    QCOMPARE(term.statusCode(), 5);

    QCOMPARE(dataSpy.count(), 4);
    arguments = dataSpy.at(0);
    QVERIFY(arguments.at(0).toString().endsWith(QString("echo \"Hello World\"\r\n")));
    arguments = dataSpy.at(1);
    QCOMPARE(arguments.at(0).toString(), QString("Hello World\r\n"));
    arguments = dataSpy.at(2);
    QVERIFY(arguments.at(0).toString().endsWith(QString("exit 5\r\n")));
    arguments = dataSpy.at(3);
    QCOMPARE(arguments.at(0).toString(), QString("exit\r\n"));
}

void PseudoTerminalTests::testArgZero()
{
    PseudoTerminal term;

    QSignalSpy dataSpy(&term, SIGNAL(lineReceived(QString)));

    term.start("/bin/bash");
    QTest::qWait(500);

    term.sendData("echo $0\n");
    QTest::qWait(500);

    QCOMPARE(dataSpy.count(), 2);
    QList<QVariant> arguments = dataSpy.at(1);
    QCOMPARE(arguments.at(0).toString(), QString("/bin/bash\r\n"));
}

void PseudoTerminalTests::testArgs()
{
    PseudoTerminal term;

    QSignalSpy dataSpy(&term, SIGNAL(lineReceived(QString)));
    QSignalSpy finishedSpy(&term, SIGNAL(finished(int)));

    term.start("/bin/bash", QStringList("/tmp"));
    QTest::qWait(500);

    QCOMPARE(finishedSpy.count(), 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 126);

    QCOMPARE(dataSpy.count(), 1);
    arguments = dataSpy.at(0);
    QVERIFY(arguments.at(0).toString().startsWith("/tmp: /tmp: "));
}

void PseudoTerminalTests::testDataReceived()
{
    PseudoTerminal term;

    QSignalSpy dataSpy(&term, SIGNAL(lineReceived(QString)));

    term.start("/bin/bash");
    QTest::qWait(500);

    term.sendData("exit 5\n");
    QTest::qWait(500);

    QCOMPARE(dataSpy.count(), 2);
    QList <QVariant> arguments = dataSpy.at(1);
    QCOMPARE(arguments.at(0).toString(), QString("exit\r\n"));
}

void PseudoTerminalTests::testTerminate()
{
    PseudoTerminal term;

    QSignalSpy finishedSpy(&term, SIGNAL(finished(int)));

    term.start("/bin/bash");
    QTest::qWait(500);
    QVERIFY(term.isRunning());

    QCOMPARE(finishedSpy.count(), 0);
    term.terminate();
    QTest::qWait(500);

    QCOMPARE(finishedSpy.count(), 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 0);
    QCOMPARE(term.statusCode(), 0);
}

void PseudoTerminalTests::lineReceived(const QString &data)
{
    std::cout << "New data:\n";
    std::cout << data.toStdString() << "\n";
}
