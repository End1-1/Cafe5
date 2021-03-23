#include "testn2.h"
#include <QTimer>

TestN2::TestN2(const QString &serverIP, int port, const QSslCertificate &certificate, int number, const QVariant &data):
    TestN(serverIP, port, certificate, number, data)
{

}

TestN2::~TestN2()
{
    fSslSocket->deleteLater();
    qDebug() << "~TestN2()";
}

void TestN2::run()
{
    QTimer *fTimer = new QTimer();
    connect(fTimer, &QTimer::timeout, this, &TestN2::timeout);
    fTimer->start(fData.toInt() * 1000);
    fSslSocket = new SslSocket(this);
    connect(fSslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
    fSslSocket->addCaCertificate(fCertificate);
    fSslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    fSslSocket->connectToHostEncrypted(fServerIP, fPort);
    if (fSslSocket->waitForEncrypted(30000)) {
        emit data(1, QString("Connection N%1").arg(fNumber));
    } else {

    }
}

void TestN2::timeout()
{
    sender()->deleteLater();
    emit data(0, "Normally quit");
    emit finished();
}
