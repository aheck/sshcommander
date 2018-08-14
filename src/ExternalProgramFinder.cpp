#include "ExternalProgramFinder.h"

QString ExternalProgramFinder::getSSHPath()
{
    static const QStringList paths = {
        "/usr/bin/ssh",
        "/usr/local/bin/ssh"
    };

    for (const QString& path : paths) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    return "";
}

QString ExternalProgramFinder::getSSHFSPath()
{
    static const QStringList paths = {
        "/usr/bin/sshfs",
        "/usr/local/bin/sshfs"
    };

    for (const QString& path : paths) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    return "";
}
