#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include "sslsocket.h"
#include <QString>

class SocketData {
public:
    SslSocket *fSocket;
    QByteArray fSessionId;
    int fUserId;
    QMap<int, QString> fSettings;
    QString getValue(int key) const;
};

#endif // SOCKETDATA_H
