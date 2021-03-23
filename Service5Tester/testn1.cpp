#include "testn1.h"
#include <QElapsedTimer>

TestN1::TestN1(const QString &serverIP, int port, const QSslCertificate &certificate, int number, const QVariant &data) :
    TestN(serverIP, port, certificate, number, data)
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
        s->write(fData.toMap()["http_request"].toString().toUtf8());
        if (s->waitForBytesWritten()) {
        } else {
            emit threadError(1, s->errorString());
            return;
        }
        s->waitForReadyRead();
        emit data(2, s->readAll());
        s->close();
        s->deleteLater();
        emit data(1, QString("Elapsed %1").arg(t.elapsed()));
        emit finished();
    }
}
