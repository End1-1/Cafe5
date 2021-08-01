#include "testn.h"
#include <QSslSocket>

TestN::TestN(const QString &serverIP, int port, const QSslCertificate &certificate, int number, int timeout, const QVariant &data) :
    ThreadWorker(),
    fServerIP(serverIP),
    fPort(port),
    fCertificate(certificate),
    fNumber(number),
    fTimeout(timeout),
    fData(data)
{

}

void TestN::err(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    emit threadError(-1, static_cast<QSslSocket*>(sender())->errorString());
}
