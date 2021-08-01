#include "testchat.h"
#include <QElapsedTimer>

TestChat::TestChat(const QString &serverIP, int port, const QSslCertificate &certificate, int number, int timeout, const QVariant &data) :
    TestN(serverIP, port, certificate, number, timeout, data)
{

}

void TestChat::run()
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

            return;
        }
        s->waitForReadyRead();
        emit data(2, s->readAll());
        s->close();
        emit data(1, QString("Elapsed %1").arg(t.elapsed()));
        emit done();
    }
    delete s;
}
