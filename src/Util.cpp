#include "Util.h"

#define PASSWORD_KEY "71A0D49A64D2D66F6D097BF78BD496BD1932827BBB454B76E944CC3452C7D099"

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

QString Util::encryptString(const QString &plaintext)
{
    AES_ctx ctx;
    QByteArray aesKey = QByteArray::fromHex(PASSWORD_KEY);
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
    QByteArray aesKey = QByteArray::fromHex(PASSWORD_KEY);

    QByteArray data = QByteArray::fromBase64(base64CipherText.toLatin1());

    AES_init_ctx(&ctx, reinterpret_cast<const uint8_t*>(aesKey.constData()));
    AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(data.data()), data.size());

    // remove garbage IV block
    data.remove(0, 16);

    QString plaintext = QString::fromUtf8(data);
    return plaintext;
}
