#include "threadsendmessage.h"
#include "c5config.h"
#include <QThread>

ThreadSendMessage::ThreadSendMessage(QObject *parent) :
    QObject(parent)
{

}

void ThreadSendMessage::send(int recipient, const QString &message)
{
    fRecipient = recipient;
    fMessage = message;
    auto *t = new QThread();
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), this, SLOT(start()));
    connect(this, SIGNAL(finished()), t, SLOT(quit()));
    moveToThread(t);
    t->start();
}

void ThreadSendMessage::start()
{
    QString url = QString("POST /chat HTTP/1.1\r\n"
            "Content-Type: multipart/form-data; boundary=--hdhdhdjfjfjgkgk85858\r\n"
            "Content-Length: %content-length%\r\n"
            "\r\n");
    QString body = QString(
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"auth\"\r\n\r\n"
            "up\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"user\"\r\n\r\n"
            "%1\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"pass\"\r\n\r\n"
            "%2\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"a\"\r\n\r\n"
            "post\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"usermap\"\r\n\r\n"
            "1\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"recipient\"\r\n\r\n"
            "%3\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n"
            "Content-Disposition: form-data; name=\"message\"\r\n\r\n"
            "%4\r\n"
            "----hdhdhdjfjfjgkgk85858\r\n")
            .arg(__c5config.httpServerUsername(), __c5config.httpServerPassword(), QString::number(fRecipient), fMessage);
    url.replace("%content-length%", QString::number(body.toUtf8().length()));
    auto *s = new QSslSocket(this);
    //connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    s->addCaCertificate(fSslCertificate);
    s->setPeerVerifyMode(QSslSocket::VerifyNone);
    QJsonObject jo;
    s->connectToHostEncrypted(__c5config.httpServerIP(), __c5config.httpServerPort());
    if (s->waitForEncrypted(5000)) {
        s->write(url.toUtf8());
        s->write(body.toUtf8());
        if (s->waitForBytesWritten()) {
        } else {
        }
        s->waitForReadyRead();
        QString reply = s->readAll();
        int msgbodystart = reply.indexOf("\r\n\r\n") + 4;
        jo["status"] = 0;
        jo["data"] = reply.mid(msgbodystart, reply.length() - msgbodystart);
        s->close();
    } else {
        jo["status"] = 1;
        jo["data"] = s->errorString();
    }
    s->deleteLater();
    emit result(jo);
    emit finished();
}

void ThreadSendMessage::err(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    auto *s = static_cast<QTcpSocket*>(sender());
    QJsonObject jo;
    jo["status"] = 1;
    jo["data"] = s->errorString();
    emit result(jo);
    emit finished();
}
