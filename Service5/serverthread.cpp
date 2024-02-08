#include "serverthread.h"
#include "sslserver.h"
#include "sslsocket.h"
#include "socketthread.h"
#include "logwriter.h"
#include "thread.h"
#include "configini.h"
#include <QSslConfiguration>

ServerThread::ServerThread(const QString &configPath) :
    ThreadWorker(),
    fConfigPath(configPath)
{
}

ServerThread::~ServerThread()
{
    for (SslServer *s: fSslServer) {
        s->deleteLater();
    }
}

void ServerThread::run()
{
    QStringList ports = ConfigIni::value("server/port").split(",", Qt::SkipEmptyParts);
    LogWriter::write(LogWriterLevel::verbose, "", "SSL version: " + QSslSocket::sslLibraryBuildVersionString());
    LogWriter::write(LogWriterLevel::verbose, "", "SSL support: " + QString((QSslSocket::supportsSsl() ? "YES" : "NO")));
    QString certFileName = fConfigPath + "cert.pem";
    QString keyFileName = fConfigPath + "key.pem";
    for (const QString &p: ports) {
        auto *s = new SslServer();
        s->setMaxPendingConnections(10000);
        if (!s->setSslLocalCertificate(certFileName)) {
            LogWriter::write(LogWriterLevel::errors, "", QString("%1 certificate is not instaled").arg(certFileName));
        }
        if (!s->setSslPrivateKey(keyFileName)) {
            LogWriter::write(LogWriterLevel::errors, "", QString("%1 private key is not instaled").arg(keyFileName));
        }
        fSslChain = QSslCertificate::fromPath(fConfigPath + "fullchain.pem", QSsl::Pem);


        s->setSslProtocol(QSsl::TlsV1_2OrLater);
        fSslLocalCertificate = s->fSslLocalCertificate;
        fSslPrivateKey = s->fSslPrivateKey;
        fSslProtocol = s->fSslProtocol;
        if (!s->startListen(p.toInt())) {
            LogWriter::write(LogWriterLevel::errors, "", "Cannot listen port: " + QString::number(ConfigIni::value("server/port").toInt()));
            exit(1);
            return;
        }
        LogWriter::write(LogWriterLevel::errors, "", "Listen port: " + p);
        connect(s, &SslServer::connectionRequest, this, &ServerThread::newConnection);
        fSslServer.append(s);
    }
}

void ServerThread::newConnection(int socketDescriptor)
{
    auto *thread = new Thread("SocketThread");
    auto *socketThread = new SocketThread(socketDescriptor, fSslLocalCertificate, fSslPrivateKey, fSslProtocol, fSslChain);
    connect(thread, &QThread::started, socketThread, &SocketThread::run);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(socketThread, &SocketThread::finished, thread, &Thread::quit);
    socketThread->moveToThread(thread);
    thread->start();
}

