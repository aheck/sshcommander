#include "KnownHostsTests.h"

void KnownHostsTests::testIsHostnameHashingEnabledPositive1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "    #   RekeyLimit 1G 1h\n"
        "   SendEnv LANG LC_*\n"
        "   HashKnownHosts yes\n"
        "   GSSAPIAuthentication yes\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostnameHashingEnabled(filename));
}

void KnownHostsTests::testIsHostnameHashingEnabledPositive2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "#   RekeyLimit 1G 1h\n"
        "#SendEnv LANG LC_*\n"
        "HashKnownHosts yes\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostnameHashingEnabled(filename));
}

void KnownHostsTests::testIsHostnameHashingEnabledNegative1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "#   RekeyLimit 1G 1h\n"
        "SendEnv LANG LC_*\n"
        "HashKnownHosts no\n"
        "GSSAPIAuthentication yes\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(!KnownHosts::isHostnameHashingEnabled(filename));
}

void KnownHostsTests::testIsHostnameHashingEnabledNegative2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "#   RekeyLimit 1G 1h\n"
        "SendEnv LANG LC_*\n"
        "GSSAPIAuthentication yes\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(!KnownHosts::isHostnameHashingEnabled(filename));
}

bool KnownHostsTests::writeStringToFile(const QString &filename, QString &data)
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
