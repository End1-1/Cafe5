#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "threadworker.h"
#include "sslsocket.h"
#include "socketdata.h"
#include <QElapsedTimer>

class SocketThread : public ThreadWorker
{
    Q_OBJECT

public:
    SocketThread(SslSocket *sslSocket);
    ~SocketThread();

protected slots:
    virtual void run() override;

private:
    QElapsedTimer fTimer;
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
};

#endif // SOCKETTHREAD_H
