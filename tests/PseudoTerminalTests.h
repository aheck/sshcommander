#ifndef PSEUDOTERMINALTESTS_H
#define PSEUDOTERMINALTESTS_H

#include <QDateTime>
#include <QObject>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include <memory>
#include <iostream>

#include "PseudoTerminal.h"
#include "TestHelpers.h"

class PseudoTerminalTests : public QObject
{
    Q_OBJECT

public slots:
    void lineReceived(QString data);
    void errorOccured(QProcess::ProcessError error, QString message);
private slots:
    void testWithSimpleProgram();
    void testBasicOperation();
    void testArgZero();
    void testArgs();
    void testDataReceived();
    void testTerminate();
    void testWaitForFinished();
    void testWaitForFinishedTimeout();
    void testWaitForReadyRead();
    void testWaitForReadyReadTimeout();
    void testInParallel();
};

#endif
