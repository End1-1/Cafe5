#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "threadworker.h"
#include <QSslCertificate>
#include <QSslKey>

class SslServer;

class ServerThread : public ThreadWorker
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);
    ~ServerThread();

public slots:
    void run();

private:
    SslServer *fSslServer;
    const QString fConfigPath;
    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;
    QList<QSslCertificate> fSslChain;

private slots:
    void newConnection(int socketDescriptor);

};

#endif // SERVERTHREAD_H
