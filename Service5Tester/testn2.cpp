#include "testn2.h"

TestN2::TestN2(const QString &serverIP, int port, const QSslCertificate &certificate, int number):
    TestN(serverIP, port, certificate, number)
{

}

TestN2::~TestN2()
{
    qDebug() << "~TestN2()";
}

void TestN2::run()
{
    auto *s = new SslSocket(this);
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    s->addCaCertificate(fCertificate);
    s->setPeerVerifyMode(QSslSocket::VerifyNone);
    s->connectToHostEncrypted(fServerIP, fPort);
    if (s->waitForEncrypted(30000)) {
        emit data(1, QString("Connection N%1").arg(fNumber));
    } else {

    }
}
