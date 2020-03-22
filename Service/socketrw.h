#ifndef SOCKETRW_H
#define SOCKETRW_H

#include "socketdata.h"
#include <QString>

class SslSocket;

class SocketRW
{
public:
    SocketRW(SslSocket *socket);
    ~SocketRW();
    void go();

private:
    SocketData fSocketData;
};

#endif // SOCKETRW_H
