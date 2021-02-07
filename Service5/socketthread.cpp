#include "socketthread.h"

SocketThread::SocketThread(SslSocket *sslSocket) :
    ThreadWorker(),
    fSslSocket(sslSocket)
{
    fSocketType = Invalid;
    fSslSocket->setParent(0);
    fSslSocket->moveToThread(fThread);
}

SocketThread::~SocketThread()
{
    qDebug() << "~SocketThread()";
    fSslSocket->deleteLater();
}

void SocketThread::run()
{
    qRegisterMetaType <QAbstractSocket::SocketError> ("QAbstractSocket::SocketError");
    connect(fSslSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(fSslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(fSslSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void SocketThread::readyRead()
{
    /* Raw data in socket pattern, otherwise means http request */
    QByteArray ba;
    ba.append(0x03);
    ba.append(0x04);
    ba.append(0x15);

    if (fData.isEmpty()) {
        fData = fSslSocket->read(3);
        if (fData.compare(ba) == 0) {
            fSocketType = RawData;
        } else {
            fSocketType = HttpRequest;
        }
    }
    fData.append(fSslSocket->readAll());
    qDebug() << fData;
    fSslSocket->write("HTTP/1.1 200 OK\r\n\r\n<html><H1>pizda</H1></html>");
    fSslSocket->close();
}

void SocketThread::disconnected()
{
    qDebug() << "disconnected";
    emit finished();
}

void SocketThread::error(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    emit threadError(1, fSslSocket->errorString());
    disconnect();
}
