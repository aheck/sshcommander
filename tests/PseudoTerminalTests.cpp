#include "PseudoTerminalTests.h"

void PseudoTerminalTests::testBasicOperation()
{
    PseudoTerminal term;

    connect(&term, &PseudoTerminal::dataReceived, this, &PseudoTerminalTests::dataReceived);
    QSignalSpy dataSpy(&term, SIGNAL(dataReceived(QString)));
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

    QCOMPARE(dataSpy.count(), 3);
    arguments = dataSpy.at(1);
    QVERIFY(arguments.at(0).toString().startsWith(QString("echo \"Hello World\"\r\nHello World\r\n")));
    arguments = dataSpy.at(2);
    QCOMPARE(arguments.at(0).toString(), QString("exit 5\r\nexit\r\n"));
}

void PseudoTerminalTests::dataReceived(const QString &data)
{
    std::cout << "New data:\n";
    std::cout << data.toStdString() << "\n";
}
