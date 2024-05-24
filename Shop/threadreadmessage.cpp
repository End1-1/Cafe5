#include "threadreadmessage.h"
#include "c5config.h"

ThreadReadMessage::ThreadReadMessage(const QString &list) :
    ThreadWorker()
{
    fList = list;
}

void ThreadReadMessage::run()
{
    if (__c5config.httpServerUsername().isEmpty()) {
        emit finished();
        return;
    }
    QString url = QString("GET /chat?auth=up&a=read&user=%1&pass=%2&list=%3 HTTP/1.1\r\n\r\n")
                  .arg(__c5config.httpServerUsername())
                  .arg(__c5config.httpServerPassword())
                  .arg(fList);
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
        s->deleteLater();
        emit finished();
    }
}

void ThreadReadMessage::err(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    emit threadError(1, static_cast<QSslSocket *>(sender())->errorString());
}
