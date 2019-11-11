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
    void testIsHostnameHashingEnabled();

private:
    bool writeStringToFile(const QString &filename, QString &data);
};

#endif
