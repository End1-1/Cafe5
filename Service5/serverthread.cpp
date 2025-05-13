#include "serverthread.h"
#include "logwriter.h"
#include "configini.h"
#include "c5searchengine.h"
#include "database.h"
#include "armsoft.h"
#include "waiter.h"
#include "fileversion.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>
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

QString ServerThread::getDbList(const QJsonObject &jdoc)
{
    Database db;
    QJsonObject jrep;
    if (!db.open("127.0.0.1", "service5", "root", "root5")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }
    db[":fkey"] = jdoc["server_key"].toString();
    db.exec("select * from dblist where fkey=:fkey");
    QJsonArray ja;
    while (db.next()) {
        QJsonObject jt;
        jt["name"] = db.string("fcomment");
        jt["database"] = db.string("fpath");
        jt["settings"] = db.string("fsettings");
        ja.append(jt);
    }
    jrep["result"] = ja;
    jrep["errorCode"] = 0;
    return  QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString ServerThread::getConnection(const QJsonObject &jdoc)
{
    Database db;
    QString repMsg;
    QJsonObject jrep;
    if (!db.open("127.0.0.1", "service5", "root", "root5")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        return repMsg;
    }
    db[":fkey"] = jdoc["server_key"].toString();
    db[":fname"] = jdoc["connection_name"].toString();
    db.exec("select * from dblist where fkey=:fkey and fcomment=:fname");
    QJsonObject jt;
    if (db.next()) {
        jt["name"] = db.string("fcomment");
        jt["database"] = db.string("fpath");
        jt["settings"] = db.string("fsettings");
        jrep["result"] = jt;
        jrep["errorCode"] = 0;
    } else {
        jrep["errorMessage"] = "Connection name not found";
        jrep["errorCode"] = 1;
    }
    repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    return repMsg;
}

QString ServerThread::handleDll(const QJsonObject &jdoc, const QString &command)
{
#ifdef Q_OS_WIN
    QString libraryPath = QString("%1/handlers/%2.dll").arg(qApp->applicationDirPath(), command);
#else
    QString libraryPath = QString("/opt/service5/%1.so").arg(command);
#endif
    QLibrary l(libraryPath);
    QString repMsg;
    QJsonObject jrep;
    if (!l.load()) {
        jrep["errorCode"] = 2;
        jrep["errorMessage"] = "Library not found";
        jrep["library"] = libraryPath;
        repMsg =  QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    } else {
        handler h = reinterpret_cast<handler>(l.resolve(jdoc["handler"].toString().toLatin1().data()));
        if (!h) {
            jrep["errorCode"] = 3;
            jrep["errorMessage"] = "Handler not found";
            jrep["handler"] = jdoc["handler"].toString();
            repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        } else {
            QJsonObject jresponse = jrep;
            QString err;
            if (!h(jdoc, jresponse, err)) {
                jrep["errorCode"] = jresponse.value("errorCode").toInt(4);
                jrep["errorMessage"] = err;
                repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
            }
        }
    }
    l.unload();
    return repMsg;
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

QString ServerThread::updateHotelCache(const QJsonObject &jdoc)
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
    return QJsonDocument({"errorCode", 0}).toJson(QJsonDocument::Compact);
}

QString ServerThread::armsoft(const QJsonObject &jdoc)
{
    ArmSoft as(jdoc["params"].toObject());
    QJsonObject jret;
    QString err;
    jret["idontknowe"] = as.exportToAS(err);
    jret["errorCode"] = err.isEmpty() ? 0 : 1;
    jret["errorMessage"] = err;
    return QJsonDocument(jret).toJson(QJsonDocument::Compact);
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
    QWebSocket *ws = static_cast<QWebSocket *>(sender());
    QString messageCopy = msg;
    QPointer<QWebSocket> wsCopy(ws);
    auto uuid = Database::uuid();
    if (msg == "ping") {
        wsCopy->sendTextMessage("pong");
    } else {
        QJsonObject jdoc = QJsonDocument::fromJson(messageCopy.toUtf8()).object();
        if (jdoc["command"].toString() == "register_socket") {
            registerSocket(jdoc, ws);
            return;
        } else if (jdoc["command"].toString()  == "unregister_socket") {
            unregisterSocket(jdoc, ws);
            return;
        } else {
            LogWriter::write(LogWriterLevel::verbose, "REQUEST " + uuid, msg);
            QThreadPool::globalInstance()->start(QRunnable::create([this, jdoc, wsCopy, uuid]() {
                if (!wsCopy)
                    return;
                QJsonObject jresponse;
                QString repMsg;
                handleCommand(jdoc, repMsg);
                if (!wsCopy)
                    return;
                QMetaObject::invokeMethod(wsCopy, [wsCopy, repMsg, uuid]() {
                    if (wsCopy) {
                        LogWriter::write(LogWriterLevel::verbose, "REPLY " + uuid, repMsg);
                        wsCopy->sendTextMessage(repMsg);
                    }
                }, Qt::QueuedConnection);
            }));
        }
    }
}

void ServerThread::handleCommand(const QJsonObject &jdoc, QString &repMsg)
{
    QJsonObject jrep;
    if (jdoc.isEmpty()) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Empty or invalid JSON";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    } else {
        QString command = jdoc["command"].toString().toLower();
        if (command == "who?") {
            jrep["me"] = "Service5";
            jrep["version"] = FileVersion::getVersionString();
            repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        } else if (command == "get_db_list") {
            repMsg = getDbList(jdoc);
        } else if (command == "get_connection") {
            repMsg = getConnection(jdoc);
        } else if (command == "waiter") {
            Waiter w(jdoc);
            repMsg = w.process();
        } else if (command == "hotel_cache_update") {
            repMsg = updateHotelCache(jdoc);
        } else if (command == "search_engine_reload") {
            C5SearchEngine::init(fDbList);
            LogWriter::write(LogWriterLevel::verbose, "Initialized databases", fDbList.join(","));
        } else if (command == "search_text") {
            repMsg = C5SearchEngine::mInstance->search(jdoc);
        } else if (command == "search_partner") {
            repMsg = C5SearchEngine::mInstance->searchPartner(jdoc);
        } else if (command == "search_goods_groups") {
            repMsg = C5SearchEngine::mInstance->searchGoodsGroups(jdoc);
        } else if (command == "search_goods") {
            repMsg = C5SearchEngine::mInstance->searchGoods(jdoc);
        } else if (command == "search_store") {
            repMsg = C5SearchEngine::mInstance->searchStore(jdoc);
        } else if (command == "search_update_partner_cache") {
            repMsg = C5SearchEngine::mInstance->searchUpdatePartnerCache(jdoc);
        } else if (command == "armsoft") {
            repMsg = armsoft(jdoc);
        } else {
            repMsg = handleDll(jdoc, command);
        }
    }
}
