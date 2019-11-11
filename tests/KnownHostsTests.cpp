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
        "HashKnownHosts yes";

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

void KnownHostsTests::testIsHostInKnownHostsFile()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
}

void KnownHostsTests::testAddHostToKnownHostsFile()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString filename = tmpFile.fileName();
    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));

    QVERIFY(KnownHosts::addHostToKnownHostsFile("192.168.1.1", "ssh-rsa", "KW1kG4Ql9+Usdk1M4ig1jQ==", filename));

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));
}

void KnownHostsTests::testRemoveHostFromKnownHostsFile()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
    QVERIFY(KnownHosts::removeHostFromKnownHostsFile("192.168.0.192", filename));
    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
}

void KnownHostsTests::testReplaceHostInKnownHostsFile()
{
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
