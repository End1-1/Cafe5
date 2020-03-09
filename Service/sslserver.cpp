#include "sslserver.h"
#include "logwriter.h"
#include "sslsocket.h"
#include <QFile>

SslServer::SslServer(QObject *parent) :
    QTcpServer(parent)
{

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

void SslServer::incomingConnection(qintptr handle)
{
    LogWriter::write(10, 1, "", "SslServer::incomingConnection");
    SslSocket *sslSocket = new SslSocket(this);
    sslSocket->setSocketDescriptor(handle);
    sslSocket->setLocalCertificate(fSslLocalCertificate);
    sslSocket->setPrivateKey(fSslPrivateKey);
    sslSocket->setProtocol(fSslProtocol);
    sslSocket->startServerEncryption();
    this->addPendingConnection(sslSocket);
}
