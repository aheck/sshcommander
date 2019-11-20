#include "TestHelpers.h"

bool TestHelpers::writeStringToFile(const QString &filename, QString &data)
{
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream stream(&file);
    stream << (data);

    file.close();

    return true;
}

QString TestHelpers::readFileContents(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }

    QTextStream stream(&file);

    return stream.readAll();
}

QString TestHelpers::genFileChecksum(const QString &filename, QCryptographicHash::Algorithm algo)
{
    QFile f(filename);

    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(algo);
        if (hash.addData(&f)) {
            return hash.result().toHex();
        }
    }

    return "";
}
