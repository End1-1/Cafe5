#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "sslsocket.h"
#include "socketdata.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QSslCertificate>
#include <QSslKey>

class SocketThread : public QObject
{
    Q_OBJECT

public:
    SocketThread(int handle, const QSslCertificate &cert, const QSslKey &key, QSsl::SslProtocol proto,
                 const QList<QSslCertificate> chain);
    ~SocketThread();
    SslSocket *fSslSocket;

public slots:
    virtual void run();

private:
    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;
    QList<QSslCertificate> fSslChain;

    QTimer *fTimeoutControl;
    QElapsedTimer fTimer;
    int fSocketDescriptor;
    bool fDoNotRemovePluginSocket;
    quint32 fPreviouseMessageNumber;
    quint32 fMessageNumber;
    quint32 fMessageId;
    QByteArray fData;
    QString fMethodString;
    QHash<QString, DataAddress> fHttpHeader;
    QHash<QString, DataAddress> fRequestBody;
    ContentType fContentType;
    int fHeaderLength;
    int fContentLenght;
    quint16 fMessageListData;
    QString fBoundary;
    void httpRequest();
    HttpRequestMethod parseRequest(HttpRequestMethod &requestMethod, QString &httpVersion, QString &route);
    void parseBody(const QString &request, int offset);
    QString header(const QString &name) const;
    QString request(const QString &name) const;
    QString data(const DataAddress &da) const;

private slots:
    void timeoutControl();
    void readyRead();
    void writeToSocket(const QByteArray &d);
    void disconnected();
    void error(QAbstractSocket::SocketError err);

signals:
    void finished();
};

#endif // SOCKETTHREAD_H
