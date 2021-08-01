#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QSslCertificate>
#include <QSslKey>

class SslServer;

class ServerThread : public QObject
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);
    ~ServerThread();


private:
    QThread fThread;
    SslServer *fSslServer;
    const QString fConfigPath;
    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;

private slots:
    void newConnection(int socketDescriptor);

signals:

};

#endif // SERVERTHREAD_H
