#include "serverthread.h"
#include "logwriter.h"
#include "configini.h"
#include "c5searchengine.h"
#include "database.h"
#include "armsoft.h"
#include "fileversion.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QApplication>
#include <QLibrary>
#include <QMutex>

QMutex mSocketMutex;

typedef bool ( *handler) (const QJsonObject &, QJsonObject &, QString &);
ServerThread::ServerThread(const QString &configPath) :
    QObject(),
    fConfigPath(configPath)
{
    Database db;
    if (db.open("127.0.0.1", "service5", "root", "root5")) {
        db.exec("select fname from dblist");
        while (db.next()) {
            fDbList.append(db.string("fname"));
        }
        C5SearchEngine::init(fDbList);
        LogWriter::write(LogWriterLevel::verbose, "Initialized databases", fDbList.join(","));
    } else {
        LogWriter::write(LogWriterLevel::errors, "Failed initialization of service5 database", db.lastDbError());
    }
}

ServerThread::~ServerThread()
{
    qDebug() << "ServerThread destructor";
    fServer->deleteLater();
}

void ServerThread::run()
{
    int port = ConfigIni::value("server/port").toInt();
    if (!port) {
        port = 10002;
    }
    fServer = new QWebSocketServer("BreezeServer", QWebSocketServer::NonSecureMode, this);
    // fServer->setMaxPendingConnections(10000);
    if (!fServer->listen(QHostAddress::Any, port)) {
        LogWriter::write(LogWriterLevel::errors, "",
                         "Cannot listen port: " + QString::number(port) + " " + fServer->errorString());
        exit(1);
        return;
    }
    connect(fServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    LogWriter::write(LogWriterLevel::verbose, "", QString("Listen port: %1").arg(port));
}

void ServerThread::registerSocket(const QJsonObject &jdoc, QWebSocket *ws)
{
    SocketStruct ss;
    ss.socket = ws;
    ss.socketType = jdoc["socket_type"].toInt();
    ss.database = jdoc["database"].toString();
    ss.userid = jdoc["userid"].toInt();
    fSockets.append(ss);
}

void ServerThread::unregisterSocket(const QJsonObject &jdoc, QWebSocket *ws)
{
    qDebug() << "unregister_socket";
}

void ServerThread::updateHotelCache(const QJsonObject &jdoc, QWebSocket *ws)
{
    QMutexLocker ml( &mSocketMutex);
    for (const SocketStruct &ss : qAsConst(fSockets)) {
        if (ss.socketType != hotel) {
            continue;
        }
        if (ss.database != jdoc["database"].toString()) {
            continue;
        }
        ss.socket->sendTextMessage(QJsonDocument(jdoc).toJson(QJsonDocument::Compact));
    }
}

void ServerThread::armsoft(const QJsonObject &jdoc, QWebSocket *ws)
{
    ArmSoft as(jdoc["params"].toObject());
    QJsonObject jret;
    QString err;
    jret["idontknowe"] = as.exportToAS(err);
    jret["errorCode"] = err.isEmpty() ? 0 : 1;
    jret["errorMessage"] = err;
    ws->sendTextMessage(QJsonDocument(jret).toJson(QJsonDocument::Compact));
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
                     .arg(realIp)
                     .arg(ws->peerName())
                     .arg(ws->origin()));
    connect(ws, &QWebSocket::textMessageReceived, this, &ServerThread::onTextMessage);
    connect(ws, &QWebSocket::disconnected, this, [ws]() {
        LogWriter::write(LogWriterLevel::verbose, "", "disconnected" + ws->peerName() + " " + ws->origin());
    });
    fSockets.append(SocketStruct{ws,  0,  0,  ""});
}

void ServerThread::onDisconnected()
{
    QMutexLocker ml( &mSocketMutex);
    QWebSocket *ws = qobject_cast<QWebSocket *>(sender());
    if (!ws) {
        return;
    }
    LogWriter::write(LogWriterLevel::verbose, "", "disconnected: " + ws->peerName() + " " + ws->origin());
    auto it = std::remove_if(fSockets.begin(), fSockets.end(), [ws](const SocketStruct &s) {
        return s.socket == ws;
    });
    if (it != fSockets.end()) {
        fSockets.erase(it, fSockets.end());
    }
    ws->deleteLater();
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
    if (msg.toLower() == "who?") {
        jrep["me"] = "Service5";
        jrep["version"] = FileVersion::getVersionString();
        ws->sendTextMessage( QJsonDocument(jrep).toJson(QJsonDocument::Compact));
        return;
    }
    if (jdoc["command"].toString() == "register_socket") {
        registerSocket(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "unregister_socket") {
        unregisterSocket(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "hotel_cache_update") {
        updateHotelCache(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_engine_reload") {
        C5SearchEngine::init(fDbList);
        LogWriter::write(LogWriterLevel::verbose, "Initialized databases", fDbList.join(","));
        return;
    }
    if (jdoc["command"].toString() == "search_text") {
        C5SearchEngine::mInstance->search(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_partner") {
        C5SearchEngine::mInstance->searchPartner(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_goods_groups") {
        C5SearchEngine::mInstance->searchGoodsGroups(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_goods") {
        C5SearchEngine::mInstance->searchGoods(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_store") {
        C5SearchEngine::mInstance->searchStore(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "search_update_partner_cache") {
        C5SearchEngine::mInstance->searchUpdatePartnerCache(jdoc, ws);
        return;
    }
    if (jdoc["command"].toString() == "armsoft") {
        armsoft(jdoc, ws);
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
#ifdef Q_OS_WIN
    QString libraryPath = QString("%1/handlers/%2.dll").arg(qApp->applicationDirPath(), command);
#else
    QString libraryPath = QString("/opt/service5/%1.so").arg(command);
#endif
    QLibrary l(libraryPath);
    if (!l.load()) {
        jrep["errorCode"] = 2;
        jrep["errorMessage"] = "Library not found";
        jrep["library"] = libraryPath;
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        ws->sendTextMessage(repMsg);
        return;
    }
    handler h = reinterpret_cast<handler>(l.resolve(jdoc["handler"].toString().toLatin1().data()));
    if (!h) {
        jrep["errorCode"] = 3;
        jrep["errorMessage"] = "Handler not found";
        jrep["handler"] = jdoc["handler"].toString();
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
        l.unload();
        return;
    }
    l.unload();
    jresponse["requestId"] = jdoc["requestId"].toInt();
    repMsg = QJsonDocument(jresponse).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    ws->sendTextMessage(repMsg);
}
