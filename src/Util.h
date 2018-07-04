#ifndef UTIL_H
#define UTIL_H

#include <QString>

class Util
{
public:
    static QString dirname(QString path);
    static QString basename(QString path);
    static double roundBytesUp(double numBytes);
    static QString formatBytes(uint64_t numBytes);
};

#endif
