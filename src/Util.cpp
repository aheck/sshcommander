#include "Util.h"

QString Util::dirname(QString path)
{
    while (path.endsWith("/") && path != "/") {
        path.chop(1);
    }

    if (path == "/") {
        return path;
    }

    int pos = path.lastIndexOf("/");
    if (pos == -1) {
        return ".";
    }

    path.truncate(pos);
    if (path.length() == 0) {
        path = "/";
    }
    return path;
}

QString Util::basename(QString path)
{
    while (path.endsWith("/") && path != "/") {
        path.chop(1);
    }

    if (path == "/") {
        return path;
    }

    int pos = path.lastIndexOf("/");
    if (pos == -1) {
        return path;
    }

    return path.remove(0, pos + 1);
}

inline double Util::roundBytesUp(double numBytes)
{
    // We display the byte value to the first position after the decimal point
    // and we always want to round the bytes to the next higher value.
    //
    // Claiming files to be slightly bigger than they really are is better than
    // to claim that they are slightly smaller.
    return numBytes + 0.04;
}

QString Util::formatBytes(uint64_t numBytes)
{
    double gb = 1024 * 1024 * 1024;
    double mb = 1024 * 1024;
    double kb = 1024;
    double result;

    if (numBytes >= gb) {
        result = Util::roundBytesUp(numBytes / gb);
        return QString::number(result, 'f', 1) + " GB";
    }

    if (numBytes >= mb) {
        result = Util::roundBytesUp(numBytes / mb);
        return QString::number(result, 'f', 1) + " MB";
    }

    if (numBytes >= kb) {
        result = Util::roundBytesUp(numBytes / kb);
        return QString::number(result, 'f', 1) + " KB";
    }

    return QString::number(numBytes) + " bytes";
}
