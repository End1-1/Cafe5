#include "testn1.h"
#include <QElapsedTimer>

TestN1::TestN1(const QString &serverIP, int port, const QSslCertificate &certificate, int number) :
    TestN(serverIP, port, certificate, number)
{

}

TestN1::~TestN1()
{
    qDebug() << "~TestN1()";
}

void TestN1::run()
{
    QElapsedTimer t;
    t.start();
    auto *s = new SslSocket(this);
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    s->addCaCertificate(fCertificate);
    s->setPeerVerifyMode(QSslSocket::VerifyNone);
    s->connectToHostEncrypted(fServerIP, fPort);
    if (s->waitForEncrypted(30000)) {
        s->write(QString("GET / HTTP/1.1\r\n").toUtf8());
        if (s->waitForBytesWritten()) {
        } else {
            emit threadError(1, s->errorString());
            return;
        }
        s->waitForReadyRead();
        emit data(2, s->readAll());
        s->close();
        s->deleteLater();
        emit data(1, t.elapsed());
        emit finished();
    }
}
