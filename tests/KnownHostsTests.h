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

class KnownHostsTests : public QObject
{
    Q_OBJECT

private slots:
    void testIsHostnameHashingEnabledPositive1();
    void testIsHostnameHashingEnabledPositive2();
    void testIsHostnameHashingEnabledNegative1();
    void testIsHostnameHashingEnabledNegative2();

    void testIsHostInKnownHostsFilePositive1();
    void testIsHostInKnownHostsFilePositive2();
    void testIsHostInKnownHostsFileNegative1();

    void testAddHostToKnownHostsFile();

    void testRemoveHostFromKnownHostsFile();

    void testReplaceHostInKnownHostsFile();

private:
    bool writeStringToFile(const QString &filename, QString &data);
    QString readFileContents(const QString &filename);
};

#endif
