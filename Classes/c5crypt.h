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
};

#endif // C5CRYPT_H
