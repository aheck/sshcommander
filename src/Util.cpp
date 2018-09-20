#include "Util.h"

#include "MainWindow.h"

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

QByteArray Util::generateRandomBytes(unsigned int numBytes)
{
    QByteArray bytes;

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 255);

    for (int i = 0; i < numBytes; i++) {
        char randomByte = dist(rd);
        bytes.append(randomByte);
    }

    return bytes;
}

QByteArray Util::getAesKey()
{
    QByteArray key;

    key.append(0x71);
    key.append(0xA0);
    key.append(0xD4);
    key.append(0x9A);
    key.append(0x64);
    key.append(0xD2);
    key.append(0xD6);
    key.append(0x6F);
    key.append(0x6D);
    key.append(0x09);
    key.append(0x7B);
    key.append(0xF7);
    key.append(0x8B);
    key.append(0xD4);
    key.append(0x96);
    key.append(0xBD);
    key.append(0x19);
    key.append(0x32);
    key.append(0x82);
    key.append(0x7B);
    key.append(0xBB);
    key.append(0x45);
    key.append(0x4B);
    key.append(0x76);
    key.append(0xE9);
    key.append(0x44);
    key.append(0xCC);
    key.append(0x34);
    key.append(0x52);
    key.append(0xC7);
    key.append(0xD0);
    key.append(0x99);

    return key;
}

QString Util::encryptString(const QString &plaintext)
{
    AES_ctx ctx;
    QByteArray aesKey = Util::getAesKey();
    QByteArray data = plaintext.toUtf8();

    // add padding
    int modulo = data.size() % 16;
    if (modulo != 0) {
        int numMissingBytes = 16 - modulo;
        data.append(numMissingBytes, '\0');
    }

    // We don't set a real IV but instead we prepend a garbage IV block to the
    // data which we throw away when decrypting. This ensures that identical
    // plaintexts encrypt to different ciphertexts and it spares us the pain
    // of having to return and store the IV alongside the ciphertext.
    QByteArray iv = Util::generateRandomBytes(16);
    data.prepend(iv);

    AES_init_ctx(&ctx, reinterpret_cast<const uint8_t*>(aesKey.constData()));
    AES_CBC_encrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(data.data()), data.size());

    return data.toBase64();
}

QString Util::decryptString(const QString &base64CipherText)
{
    AES_ctx ctx;
    QByteArray aesKey = Util::getAesKey();

    QByteArray data = QByteArray::fromBase64(base64CipherText.toLatin1());

    AES_init_ctx(&ctx, reinterpret_cast<const uint8_t*>(aesKey.constData()));
    AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(data.data()), data.size());

    // remove garbage IV block
    data.remove(0, 16);

    QString plaintext = QString::fromUtf8(data);
    return plaintext;
}

QMainWindow* Util::getQMainWindow()
{
    for (QWidget *widget : qApp->topLevelWidgets()) {
        QMainWindow* mainWindow = qobject_cast<MainWindow*>(widget);
        if (mainWindow) {
            return mainWindow;
        }
    }

    return nullptr;
}

MainWindow* Util::getMainWindow()
{
    QMainWindow *widget = Util::getQMainWindow();
    return qobject_cast<MainWindow*>(widget);
}
