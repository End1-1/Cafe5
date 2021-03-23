#include "sslsocket.h"
#include <QUuid>

SslSocket::SslSocket(QObject *parent) :
    QSslSocket(parent)
{
    fUuid = QUuid::createUuid().toString();
}

SslSocket::~SslSocket()
{
#ifdef QT_DEBUG
    qDebug() << "~SslSocket";
#endif
}
