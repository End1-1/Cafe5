#include "serverthread.h"
#include "sslserver.h"
#include "sslsocket.h"
#include "socketthread.h"
#include "logwriter.h"
#include "debug.h"
#include "thread.h"
#include "monitoringwindow.h"
#include <QApplication>

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
    MonitoringWindow::connectSender(this);
    qDebug() << "SSL version: " << QSslSocket::sslLibraryBuildVersionString();
    QString certFileName = fConfigPath + "cert.pem";
    QString keyFileName = fConfigPath + "key.pem";
    fSslServer = new SslServer();
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
    fSslServer->startListen();
    connect(fSslServer, &SslServer::connectionRequest, this, &ServerThread::newConnection);
}

void ServerThread::newConnection(int socketDescriptor)
{
    auto *thread = new Thread();
    auto *socketThread = new SocketThread(socketDescriptor, fSslLocalCertificate, fSslPrivateKey, fSslProtocol);
    connect(thread, &QThread::started, socketThread, &SocketThread::run);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(socketThread, &SocketThread::endOfWork, thread, &QThread::quit);
    thread->start();
}
