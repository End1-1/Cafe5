#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include "socketdata.h"
#include <QByteArray>
#include <QString>
#include <QDebug>

class SslSocket;

class SocketHandler
{
public:
    explicit SocketHandler(SocketData *sd, QByteArray &data);
    virtual void processData() = 0;
    virtual bool closeConnection();
    qint32 fResponseCode;

    template<typename T>
    static T *create(SocketData *sd, QByteArray &data) {
        T *t = new T(sd, data);
        return t;
    }

protected:
    QByteArray &fData;
    QString fPeerAddress;
    SocketData *fSocketData;
};

#endif // SOCKETHANDLER_H
