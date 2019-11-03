#ifndef PSEUDOTERMINALTESTS_H
#define PSEUDOTERMINALTESTS_H

#include <QObject>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include <memory>
#include <iostream>

#include "PseudoTerminal.h"

class PseudoTerminalTests : public QObject
{
    Q_OBJECT

public slots:
    void dataReceived(const QString &data);
private slots:
    void testBasicOperation();
};

#endif
