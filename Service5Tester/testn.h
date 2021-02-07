#ifndef TESTN_H
#define TESTN_H

#include "threadworker.h"
#include "sslsocket.h"

class TestN : public ThreadWorker
{
    Q_OBJECT
public:
    explicit TestN(const QString &serverIP, int port, const QSslCertificate &certificate, int number);


protected:
    int fNumber;
    QString fServerIP;
    int fPort;
    const QSslCertificate &fCertificate;

protected slots:
    void err(QAbstractSocket::SocketError e);
};

#endif // TESTN_H
