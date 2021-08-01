#include "serverthread.h"
#include "sslserver.h"
#include "sslsocket.h"
#include "socketthread.h"
#include "logwriter.h"
#include "debug.h"
#include "monitoringwindow.h"
#include <QApplication>

ServerThread::ServerThread(const QString &configPath) :
    QObject(),
    fConfigPath(configPath)
{
    qDebug() << thread() << fThread.thread();
    MonitoringWindow::connectSender(this);
    qDebug() << "SSL version: " << QSslSocket::sslLibraryBuildVersionString();
    QString certFileName = fConfigPath + "cert.pem";
    QString keyFileName = fConfigPath + "key.pem";
    fSslServer = new SslServer();
    qDebug() << "SSL SERVER THREAD BEFORE MOVE" << fSslServer->thread();
    fSslServer->moveToThread(&fThread);
    qDebug() << "SSL SERVER THREAD AFTER MOVE" << fSslServer->thread();
    fSslServer->setMaxPendingConnections(10000);
    if (!fSslServer->setSslLocalCertificate(certFileName)) {
        LogWriter::write(10, 1, "", QString("%1 certificate is not instaled").arg(certFileName));
    }
    if (!fSslServer->setSslPrivateKey(keyFileName)) {
        LogWriter::write(10, 1, "", QString("%1 private key is not instaled").arg(keyFileName));
    }
    fSslServer->setSslProtocol(QSsl::TlsV1_2OrLater);
    fSslLocalCertificate = fSslServer->fSslLocalCertificate;
    fSslPrivateKey = fSslServer->fSslPrivateKey;
    fSslProtocol = fSslServer->fSslProtocol;
    connect(&fThread, &QThread::started, fSslServer, &SslServer::startListen);
    connect(fSslServer, &SslServer::connectionRequest, this, &ServerThread::newConnection);
    fThread.start();
    qDebug() << thread() << fThread.thread();
}

ServerThread::~ServerThread()
{
    fSslServer->deleteLater();
}

void ServerThread::newConnection(int socketDescriptor)
{
    auto *socketThread = new SocketThread(socketDescriptor, fSslLocalCertificate, fSslPrivateKey, fSslProtocol);
    socketThread->start();
}
