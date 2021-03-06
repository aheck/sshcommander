#ifndef KNOWNHOSTSTESTS_H
#define KNOWNHOSTSTESTS_H

#include <QObject>

#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QtTest/QtTest>
#include <QSignalSpy>

#include <memory>
#include <iostream>

#include "KnownHosts.h"
#include "TestHelpers.h"

class KnownHostsTests : public QObject
{
    Q_OBJECT

private slots:
    void testIsHostnameHashingEnabledPositive1();
    void testIsHostnameHashingEnabledPositive2();
    void testIsHostnameHashingEnabledNegative1();
    void testIsHostnameHashingEnabledNegative2();

    void testIsHostInKnownHostsFilePositive1();
    void testIsHostInKnownHostsFilePlain();
    void testIsHostInKnownHostsFilePositive2();
    void testIsHostInKnownHostsFileNegative1();

    void testAddHostToKnownHostsFile1();
    void testAddHostToKnownHostsFile2();

    void testRemoveHostFromKnownHostsFile1();
    void testRemoveHostFromKnownHostsFile2();

    void testReplaceHostInKnownHostsFile1();
    void testReplaceHostInKnownHostsFile2();

private:
    static const QString plainKeyEntry1;
    static const QString hashedKeyEntry1;
    static const QString hashedKeyEntry2;
};

#endif
