#include "testn.h"

TestN::TestN(const QString &serverIP, int port, const QSslCertificate &certificate, int number) :
    ThreadWorker(),
    fServerIP(serverIP),
    fPort(port),
    fCertificate(certificate),
    fNumber(number)
{

}

void TestN::err(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    emit threadError(1, static_cast<SslSocket*>(sender())->errorString());
}
