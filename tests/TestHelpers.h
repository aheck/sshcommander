#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>

#include <memory>
#include <iostream>

class TestHelpers
{
public:
    static bool writeStringToFile(const QString &filename, QString &data);
    static QString readFileContents(const QString &filename);
    static QString genFileChecksum(const QString &filename, QCryptographicHash::Algorithm algo);
};

#endif
