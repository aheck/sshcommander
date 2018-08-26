/*****************************************************************************
 *
 * Util is a collection of generic static methods which can be useful in
 * different places in the code.
 *
 ****************************************************************************/

#ifndef UTIL_H
#define UTIL_H

#include <random>

#include <QString>
#include <QByteArray>

#include <libs/tinyaes/aes.hpp>

class Util
{
public:
    static QString dirname(QString path);
    static QString basename(QString path);
    static double roundBytesUp(double numBytes);
    static QString formatBytes(uint64_t numBytes);
    static QByteArray generateRandomBytes(unsigned int numBytes);
    static QString encryptString(const QString &password);
    static QString decryptString(const QString &base64CipherText);
};

#endif
