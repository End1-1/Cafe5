#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QSslSocket>

class SslSocket : public QSslSocket
{
    Q_OBJECT

public:
    SslSocket(QObject *parent = nullptr);
    QString fUuid;
};

#endif // SSLSOCKET_H
