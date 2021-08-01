#ifndef TESTN_H
#define TESTN_H

#include "threadworker.h"
#include "sslsocket.h"

class TestN : public ThreadWorker
{
    Q_OBJECT
public:
    explicit TestN(const QString &serverIP, int port, const QSslCertificate &certificate, int number, int timeout, const QVariant &data = QVariant());


protected:
    int fNumber;
    int fTimeout;
    QString fServerIP;
    int fPort;
    QVariant fData;
    const QSslCertificate &fCertificate;

protected slots:
    void err(QAbstractSocket::SocketError e);

};


#endif // TESTN_H
