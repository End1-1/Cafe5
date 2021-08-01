#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "sslsocket.h"
#include "socketdata.h"
#include <QElapsedTimer>
#include <QSslCertificate>
#include <QSslKey>
#include <QThread>

class SocketThread : public QThread
{
    Q_OBJECT

public:
    SocketThread(int handle, QSslCertificate cert, QSslKey key, QSsl::SslProtocol proto);
    ~SocketThread();

protected:
    virtual void run() override;

private:
    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;


    QElapsedTimer fTimer;
    int fSocketDescriptor;
    SslSocket *fSslSocket;
    QByteArray fData;
    SocketType fSocketType;
    QString fMethodString;
    QHash<QString, DataAddress> fHttpHeader;
    QHash<QString, DataAddress> fRequestBody;
    ContentType fContentType;
    int fHeaderLength;
    int64_t fContentLenght;
    QString fBoundary;
    void httpRequest();
    HttpRequestMethod parseRequest(HttpRequestMethod &requestMethod, QString &httpVersion, QString &route);
    void parseBody(const QString &request, int offset);
    QString header(const QString &name) const;
    QString request(const QString &name) const;
    QString data(const DataAddress &da) const;

private slots:
    void readyRead();
    void disconnected();
    void error(QAbstractSocket::SocketError err);

signals:
    void sendData(int code, const QString &, const QString &, const QVariant &);
};

#endif // SOCKETTHREAD_H
