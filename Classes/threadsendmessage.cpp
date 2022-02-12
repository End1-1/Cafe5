#include "threadsendmessage.h"
#include "c5config.h"

ThreadSendMessage::ThreadSendMessage(QObject *parent) : QObject(parent)
{

}

void ThreadSendMessage::send(int recipient, const QString &message)
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
            .arg(__c5config.httpServerUsername())
            .arg(__c5config.httpServerPassword())
            .arg(recipient)
            .arg(message);
    url.replace("%content-length%", QString::number(body.toUtf8().length()));
    auto *s = new QSslSocket(this);
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    s->addCaCertificate(fSslCertificate);
    s->setPeerVerifyMode(QSslSocket::VerifyNone);
    s->connectToHostEncrypted(__c5config.httpServerIP(), __c5config.httpServerPort());
    if (s->waitForEncrypted(5000)) {
        s->write(url.toUtf8());
        s->write(body.toUtf8());
        if (s->waitForBytesWritten()) {
        } else {
        }
        s->waitForReadyRead();
        qDebug() << s->readAll();
        s->close();
    }
    s->deleteLater();
}

void ThreadSendMessage::err(QAbstractSocket::SocketError e)
{
    qDebug() << e;
}
