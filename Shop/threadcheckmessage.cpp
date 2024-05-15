#include "threadcheckmessage.h"
#include "c5config.h"
#include "c5logsystem.h"

ThreadCheckMessage::ThreadCheckMessage()
{

}

void ThreadCheckMessage::run()
{
    QString url = QString("GET /chat?auth=up&a=get&user=%1&pass=%2 HTTP/1.1\r\n\r\n")
            .arg(__c5config.httpServerUsername(), __c5config.httpServerPassword());
    C5LogSystem::writeEvent(QString("%1 %2").arg(url, __c5config.httpServerIP()));
    auto *s = new QSslSocket(this);
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    s->setPeerVerifyMode(QSslSocket::VerifyNone);
    s->connectToHostEncrypted(__c5config.httpServerIP(), __c5config.httpServerPort());
    if (s->waitForEncrypted(5000)) {
        s->write(url.toUtf8());
        if (s->waitForBytesWritten()) {
        } else {
            emit threadError(1, s->errorString());
            return;
        }
        s->waitForReadyRead();
        emit data(2, s->readAll());
        s->close();
        emit finished();
    } else {
        emit threadError(1, s->errorString());
    }
    s->deleteLater();
}

void ThreadCheckMessage::err(QAbstractSocket::SocketError e)
{
    emit threadError(1, static_cast<QSslSocket*>(sender())->errorString());
}
