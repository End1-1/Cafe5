#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "threadworker.h"
#include "sslsocket.h"

class SocketThread : public ThreadWorker
{
    Q_OBJECT
    enum SocketType {
        Invalid,
        RawData,
        HttpRequest
    };

public:
    SocketThread(SslSocket *sslSocket);
    ~SocketThread();

protected slots:
    virtual void run() override;

private:
    SslSocket *fSslSocket;
    QByteArray fData;
    SocketType fSocketType;

private slots:
    void readyRead();
    void disconnected();
    void error(QAbstractSocket::SocketError err);
};

#endif // SOCKETTHREAD_H
