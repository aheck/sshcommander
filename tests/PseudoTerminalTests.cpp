#include "PseudoTerminalTests.h"

void PseudoTerminalTests::testBasicOperation()
{
    PseudoTerminal term;

    connect(&term, &PseudoTerminal::lineReceived, this, &PseudoTerminalTests::lineReceived);
    connect(&term, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

    QSignalSpy dataSpy(&term, SIGNAL(lineReceived(QString)));
    QSignalSpy finishedSpy(&term, SIGNAL(finished(int)));

    term.start("/bin/bash");
    QTest::qWait(500);
    term.sendData("echo \"Hello World\"\n");
    QTest::qWait(500);
    QCOMPARE(dataSpy.count(), 2);

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

    connect(&term, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

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

    connect(&term, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

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

    connect(&term, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

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

    connect(&term, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

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

void PseudoTerminalTests::testInParallel()
{
    PseudoTerminal term1;
    PseudoTerminal term2;

    connect(&term1, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);
    connect(&term2, &PseudoTerminal::errorOccured, this, &PseudoTerminalTests::errorOccured);

    QSignalSpy dataSpy1(&term1, SIGNAL(lineReceived(QString)));
    QSignalSpy dataSpy2(&term2, SIGNAL(lineReceived(QString)));
    QSignalSpy finishedSpy1(&term1, SIGNAL(finished(int)));
    QSignalSpy finishedSpy2(&term2, SIGNAL(finished(int)));

    term1.start("/bin/bash");
    QTest::qWait(500);
    QVERIFY(term1.isRunning());
    term1.sendData("echo \"Hello\"\n");
    QTest::qWait(500);
    QCOMPARE(dataSpy1.count(), 2);
    QCOMPARE(dataSpy2.count(), 0);

    term2.start("/bin/bash");
    QTest::qWait(500);
    QVERIFY(term2.isRunning());
    term2.sendData("echo \"Test\"\n");
    QTest::qWait(500);
    QCOMPARE(dataSpy1.count(), 2);
    QCOMPARE(dataSpy2.count(), 2);

    QCOMPARE(finishedSpy1.count(), 0);
    QCOMPARE(finishedSpy2.count(), 0);
    term1.terminate();
    QTest::qWait(500);
    QVERIFY(!term1.isRunning());
    QCOMPARE(finishedSpy1.count(), 1);
    QCOMPARE(finishedSpy2.count(), 0);
    QVERIFY(term2.isRunning());

    QCOMPARE(finishedSpy1.count(), 1);
    QCOMPARE(finishedSpy2.count(), 0);
    term2.terminate();
    QTest::qWait(500);
    QVERIFY(!term1.isRunning());
    QCOMPARE(finishedSpy1.count(), 1);
    QCOMPARE(finishedSpy2.count(), 1);
    QVERIFY(!term2.isRunning());
}

void PseudoTerminalTests::lineReceived(QString data)
{
    std::cout << "New data:\n";
    std::cout << data.toStdString() << "\n";
}

void PseudoTerminalTests::errorOccured(QProcess::ProcessError error, QString message)
{
    std::cerr << "Process error: " << " " << message.toStdString() << "\n";
}
