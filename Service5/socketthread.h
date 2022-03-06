#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "threadworker.h"
#include "sslsocket.h"
#include "socketdata.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QSslCertificate>
#include <QSslKey>

class SocketThread : public ThreadWorker
{
    Q_OBJECT

public:
    SocketThread(int handle, QSslCertificate cert, QSslKey key, QSsl::SslProtocol proto);
    ~SocketThread();

public slots:
    virtual void run() override;

private:
    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;


    QTimer *fTimeoutControl;
    QElapsedTimer fTimer;
    int fSocketDescriptor;
    SslSocket *fSslSocket;
    quint32 fPreviouseMessageNumber;
    quint32 fMessageNumber;
    quint32 fMessageId;
    QByteArray fData;
    SocketType fSocketType;
    QString fMethodString;
    QHash<QString, DataAddress> fHttpHeader;
    QHash<QString, DataAddress> fRequestBody;
    ContentType fContentType;
    int fHeaderLength;
    quint32 fContentLenght;
    quint16 fMessageListData;
    QString fBoundary;
    void rawRequest();
    void httpRequest();
    HttpRequestMethod parseRequest(HttpRequestMethod &requestMethod, QString &httpVersion, QString &route);
    void parseBody(quint16 msgType, const QByteArray &data);
    void parseBody(const QString &request, int offset);
    QString header(const QString &name) const;
    QString request(const QString &name) const;
    QString data(const DataAddress &da) const;

private slots:
    void timeoutControl();
    void readyRead();
    void disconnected();
    void error(QAbstractSocket::SocketError err);
};

#endif // SOCKETTHREAD_H
