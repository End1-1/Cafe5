#ifndef SOCKETRW_H
#define SOCKETRW_H

class SslSocket;

class SocketRW
{
public:
    SocketRW(SslSocket *socket);
    ~SocketRW();
    void go();

private:
    SslSocket *fSocket;
};

#endif // SOCKETRW_H
