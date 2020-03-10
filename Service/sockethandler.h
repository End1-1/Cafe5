#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QByteArray>
#include <QString>

class SslSocket;

class SocketHandler
{
public:
    explicit SocketHandler(QByteArray &data);
    void setSocket(SslSocket *socket);
    virtual void processData() = 0;
    virtual bool closeConnection();
    qint32 fResponseCode;

    template<typename T>
    static T *create(QByteArray &data) {
        T *t = new T(data);
        return t;
    }

protected:
    QByteArray &fData;
    QString fPeerAddress;
    SslSocket *fSslSocket;
};

#endif // SOCKETHANDLER_H
