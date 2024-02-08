#include "sslserver.h"
#include "logwriter.h"
#include "sslsocket.h"
#include "configini.h"
#include <QFile>

SslServer::SslServer(QObject *parent) :
    QTcpServer(parent)
{
    //connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(acceptError(QAbstractSocket::SocketError)));

}

void SslServer::setSslLocalCertificate(const QSslCertificate &s)
{
    fSslLocalCertificate = s;
}

bool SslServer::setSslLocalCertificate(const QString &path, QSsl::EncodingFormat format)
{
    QFile certificateFile(path);
    if (!certificateFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    fSslLocalCertificate = QSslCertificate(certificateFile.readAll(), format);
    return true;
}

void SslServer::setSslPrivateKey(const QSslKey &k)
{
    fSslPrivateKey = k;
}

bool SslServer::setSslPrivateKey(const QString &path, QSsl::KeyAlgorithm algorithm, QSsl::EncodingFormat format, const QByteArray &passPhrase)
{
    QFile keyFile(path);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    fSslPrivateKey = QSslKey(keyFile.readAll(), algorithm, format, QSsl::PrivateKey, passPhrase);
    return true;
}

void SslServer::setSslProtocol(QSsl::SslProtocol p)
{
    fSslProtocol = p;
}

bool SslServer::startListen(int port)
{
    return listen(QHostAddress::AnyIPv4, port);
}

void SslServer::incomingConnection(qintptr handle)
{
    emit connectionRequest((int)handle);
    //LogWriter::write(10, 1, "", "SslServer::incomingConnection prepear");
//    SslSocket *sslSocket = new SslSocket(nullptr);
//    sslSocket->setSocketDescriptor(handle);
//    sslSocket->setLocalCertificate(fSslLocalCertificate);
//    sslSocket->setPrivateKey(fSslPrivateKey);
//    sslSocket->setProtocol(fSslProtocol);
//    sslSocket->startServerEncryption();
//    addPendingConnection(sslSocket);
    //LogWriter::write(10, 1, "", "SslServer::incomingConnection ready");
}

//void SslServer::acceptError(QAbstractSocket::SocketError err)
//{
//    qDebug() << "Accept error" << err;
//}
