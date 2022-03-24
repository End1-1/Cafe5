#ifndef C5CRYPT_H
#define C5CRYPT_H

#include <QObject>

class C5Crypt : public QObject
{
    Q_OBJECT

public:
    C5Crypt();
    void cryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData);
    void decryptData(const QByteArray &k, QByteArray &inData, QByteArray &outData);
    void ede3_cbc(QByteArray &in, QByteArray &out, const QString &key, bool encrypt);
    const QString driveKey();
    const QString machineUuid();
};

#endif // C5CRYPT_H
