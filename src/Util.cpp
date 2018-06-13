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
