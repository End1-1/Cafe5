#include "serverthread.h"
#include "sslserver.h"
#include "sslsocket.h"
#include "socketthread.h"
#include "logwriter.h"
#include "debug.h"

ServerThread::ServerThread(const QString &configPath) :
    ThreadWorker(),
    fConfigPath(configPath)
{

}

ServerThread::~ServerThread()
{
    fSslServer->deleteLater();
}

void ServerThread::run()
{
    qDebug() << "SSL version: " << QSslSocket::sslLibraryBuildVersionString();
    QString certFileName = fConfigPath + "cert.pem";
    QString keyFileName = fConfigPath + "key.pem";
    fSslServer = new SslServer(this);
    fSslServer->setMaxPendingConnections(10000);
    if (!fSslServer->setSslLocalCertificate(certFileName)) {
        LogWriter::write(10, 1, "", QString("%1 certificate is not instaled").arg(certFileName));
    }
    if (!fSslServer->setSslPrivateKey(keyFileName)) {
        LogWriter::write(10, 1, "", QString("%1 private key is not instaled").arg(keyFileName));
    }
    fSslServer->setSslProtocol(QSsl::TlsV1_2OrLater);
    fSslServer->listen(QHostAddress::AnyIPv4, 10002);
    while (fSslServer->waitForNewConnection(-1)) {
        while (fSslServer->hasPendingConnections()) {
            SslSocket *sslSocket = dynamic_cast<SslSocket*>(fSslServer->nextPendingConnection());
            auto *socketThread = new SocketThread(sslSocket);
            socketThread->start();
        }
    }
    emit finished();
}
