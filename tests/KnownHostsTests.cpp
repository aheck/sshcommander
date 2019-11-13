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

void KnownHostsTests::testIsHostInKnownHostsFilePositive1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    // our config only contains the entry for the host we are looking for
    QString configContents = "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
}

void KnownHostsTests::testIsHostInKnownHostsFilePositive2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    // our config contains the entry for another host as well as the entry for
    // the host we are looking for
    QString configContents = "|1|QwqKhVmCzzHlsrSHYWDjaJ1QkZI=|mgVlL2tnMEaJlrTY8qF8WiJ3NZA= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBgrEhqiEwuSsw9vOIcJG3/exPISQe/YzQH+xjjAcbG6al7vTO/GFn/eMeIhl2DNM41HD1ssgU264Eq/+2roI74=\n"
        "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
}

void KnownHostsTests::testIsHostInKnownHostsFileNegative1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "|1|QwqKhVmCzzHlsrSHYWDjaJ1QkZI=|mgVlL2tnMEaJlrTY8qF8WiJ3NZA= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBgrEhqiEwuSsw9vOIcJG3/exPISQe/YzQH+xjjAcbG6al7vTO/GFn/eMeIhl2DNM41HD1ssgU264Eq/+2roI74=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
}

void KnownHostsTests::testAddHostToKnownHostsFile1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString filename = tmpFile.fileName();
    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));

    QVERIFY(KnownHosts::addHostToKnownHostsFile("192.168.1.1", "ssh-rsa", "KW1kG4Ql9+Usdk1M4ig1jQ==", filename));

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));
}

void KnownHostsTests::testAddHostToKnownHostsFile2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "|1|QwqKhVmCzzHlsrSHYWDjaJ1QkZI=|mgVlL2tnMEaJlrTY8qF8WiJ3NZA= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBgrEhqiEwuSsw9vOIcJG3/exPISQe/YzQH+xjjAcbG6al7vTO/GFn/eMeIhl2DNM41HD1ssgU264Eq/+2roI74=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);
    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));

    QVERIFY(KnownHosts::addHostToKnownHostsFile("192.168.1.1", "ssh-rsa", "KW1kG4Ql9+Usdk1M4ig1jQ==", filename));

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.1.1", filename));

    QString fileContents = this->readFileContents(filename);
    QVERIFY(fileContents.startsWith(configContents));
}

void KnownHostsTests::testRemoveHostFromKnownHostsFile1()
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

void KnownHostsTests::testRemoveHostFromKnownHostsFile2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString key1 = "|1|QwqKhVmCzzHlsrSHYWDjaJ1QkZI=|mgVlL2tnMEaJlrTY8qF8WiJ3NZA= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBgrEhqiEwuSsw9vOIcJG3/exPISQe/YzQH+xjjAcbG6al7vTO/GFn/eMeIhl2DNM41HD1ssgU264Eq/+2roI74=\n";
    QString configContents = key1 +
        "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));
    QVERIFY(KnownHosts::removeHostFromKnownHostsFile("192.168.0.192", filename));
    QVERIFY(!KnownHosts::isHostInKnownHostsFile("192.168.0.192", filename));

    QString fileContents = this->readFileContents(filename);
    QCOMPARE(key1, fileContents);
}

void KnownHostsTests::testReplaceHostInKnownHostsFile1()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString configContents = "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::replaceHostInKnownHostsFile("192.168.0.192", "ecdsa-sha2-nistp256", "AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZg_OTHERKEY_wBeJY=", filename));

    QString fileContents = this->readFileContents(filename);
    QVERIFY(fileContents.endsWith("ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZg_OTHERKEY_wBeJY=\n"));
}

void KnownHostsTests::testReplaceHostInKnownHostsFile2()
{
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate("qtest-knownhosts-XXXXXXX");
    QVERIFY(tmpFile.open());

    QString key1 = "|1|QwqKhVmCzzHlsrSHYWDjaJ1QkZI=|mgVlL2tnMEaJlrTY8qF8WiJ3NZA= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBgrEhqiEwuSsw9vOIcJG3/exPISQe/YzQH+xjjAcbG6al7vTO/GFn/eMeIhl2DNM41HD1ssgU264Eq/+2roI74=\n";
    QString configContents = key1 + "|1|6EUC3WtSQchRUj6EdY2P4aWCyng=|Ljfjdig29DGTbUpXl8qskIVhZco= ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZgsPaoienXILwBeJY=\n";

    QString filename = tmpFile.fileName();
    writeStringToFile(filename, configContents);

    QVERIFY(KnownHosts::replaceHostInKnownHostsFile("192.168.0.192", "ecdsa-sha2-nistp256", "AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZg_OTHERKEY_wBeJY=", filename));

    QString fileContents = this->readFileContents(filename);
    QVERIFY(fileContents.startsWith(key1));
    QVERIFY(fileContents.endsWith("ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBPnVqUGfx30dBxPboia3amukmrTCBYhPwGByh28Wl5P3dk9OZHeGidILyjqcp3rTGJlNPeZg_OTHERKEY_wBeJY=\n"));
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

QString KnownHostsTests::readFileContents(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }

    QTextStream stream(&file);

    return stream.readAll();
}
