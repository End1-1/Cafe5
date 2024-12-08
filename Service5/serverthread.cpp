#include "serverthread.h"
#include "logwriter.h"
#include "configini.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QApplication>
#include <QLibrary>

typedef bool ( *handler) (const QJsonObject &, QJsonObject &, QString &);

ServerThread::ServerThread(const QString &configPath) :
    QObject(),
    fConfigPath(configPath)
{
}

ServerThread::~ServerThread()
{
    qDebug() << "ServerThread destructor";
    fServer->deleteLater();
}

void ServerThread::run()
{
    int port = ConfigIni::value("server/port").toInt();
    fServer = new QWebSocketServer("BreezeServer", QWebSocketServer::NonSecureMode, this);
    // fServer->setMaxPendingConnections(10000);
    if (!fServer->listen(QHostAddress::Any, port)) {
        LogWriter::write(LogWriterLevel::errors, "",
                         "Cannot listen port: " + QString::number(ConfigIni::value("server/port").toInt()));
        exit(1);
        return;
    }
    connect(fServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    LogWriter::write(LogWriterLevel::verbose, "", QString("Listen port: %1").arg(port));
}

void ServerThread::onNewConnection()
{
    QWebSocket *ws = fServer->nextPendingConnection();
    QString realIp = ws->request().rawHeader("X-Forwarded-For");
    if (realIp.isEmpty()) {
        realIp = ws->peerAddress().toString();
    }
    LogWriter::write(LogWriterLevel::verbose, "",
                     QString("New connection from %1, peer name: %2, origin: %3")
                     .arg(realIp)  // Используем реальный IP
                     .arg(ws->peerName())  // Peer name, может быть пустым
                     .arg(ws->origin()));  // Origin header
    connect(ws, &QWebSocket::textMessageReceived, this, &ServerThread::onTextMessage);
    connect(ws, &QWebSocket::disconnected, this, [ws]() {
        LogWriter::write(LogWriterLevel::verbose, "", "disconnected" + ws->peerName() + " " + ws->origin());
    });
}

void ServerThread::onTextMessage(const QString &msg)
{
    LogWriter::write(LogWriterLevel::verbose, "", msg);
    auto *ws = static_cast<QWebSocket *>(sender());
    QJsonObject jdoc = QJsonDocument::fromJson(msg.toUtf8()).object();
    QJsonObject jrep;
    if (msg.toLower() == "ping") {
        ws->sendTextMessage("pong");
        return;
    }
    jrep["errorCode"] = 0;
    jrep["requestId"] = jdoc["requestId"].toInt();
    QString repMsg;
    if (!jdoc.contains("command")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Invalid json";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        ws->sendTextMessage(repMsg);
        return;
    }
    QString command = jdoc["command"].toString();
    QLibrary l(QString("%1/handlers/%2.dll").arg(qApp->applicationDirPath(), command));
    if (!l.load()) {
        jrep["errorCode"] = 2;
        jrep["errorMessage"] = "Library not found";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        ws->sendTextMessage(repMsg);
        return;
    }
    handler h = reinterpret_cast<handler>(l.resolve(jdoc["handler"].toString().toLatin1().data()));
    if (!h) {
        jrep["errorCode"] = 3;
        jrep["errorMessage"] = "Handler not found";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        ws->sendTextMessage(repMsg);
        return;
    }
    QJsonObject jresponse = jrep;
    QString err;
    if (!h(jdoc, jresponse, err)) {
        if (jresponse.contains("errorCode")) {
            jrep["errorCode"] = jresponse["errorCode"];
        } else {
            jrep["errorCode"] = 4;
        }
        jrep["errorMessage"] = err;
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        ws->sendTextMessage(repMsg);
        return;
    }
    jresponse["requestId"] = jdoc["requestId"].toInt();
    repMsg = QJsonDocument(jresponse).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    ws->sendTextMessage(repMsg);
}
