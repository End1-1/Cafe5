#include "serverthread.h"
#include "sslserver.h"
#include "sslsocket.h"
#include "socketthread.h"
#include "logwriter.h"
#include "thread.h"

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
    LogWriter::write(LogWriterLevel::verbose, "", "SSL version: " + QSslSocket::sslLibraryBuildVersionString());
    QString certFileName = fConfigPath + "cert.pem";
    QString keyFileName = fConfigPath + "key.pem";
    fSslServer = new SslServer();
    fSslServer->setMaxPendingConnections(10000);
    if (!fSslServer->setSslLocalCertificate(certFileName)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("%1 certificate is not instaled").arg(certFileName));
    }
    if (!fSslServer->setSslPrivateKey(keyFileName)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("%1 private key is not instaled").arg(keyFileName));
    }
    fSslServer->setSslProtocol(QSsl::TlsV1_2OrLater);
    fSslLocalCertificate = fSslServer->fSslLocalCertificate;
    fSslPrivateKey = fSslServer->fSslPrivateKey;
    fSslProtocol = fSslServer->fSslProtocol;
    fSslServer->startListen();
    LogWriter::write(LogWriterLevel::verbose, "", fSslServer->errorString());
    connect(fSslServer, &SslServer::connectionRequest, this, &ServerThread::newConnection);
}

void ServerThread::newConnection(int socketDescriptor)
{
    auto *thread = new Thread("SocketThread");
    auto *socketThread = new SocketThread(socketDescriptor, fSslLocalCertificate, fSslPrivateKey, fSslProtocol);
    connect(thread, &QThread::started, socketThread, &SocketThread::run);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(socketThread, &SocketThread::finished, thread, &Thread::quit);
    socketThread->moveToThread(thread);
    thread->start();
}
