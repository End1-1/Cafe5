#include "serverthread.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLibrary>
#include <QMutex>
#include <QPointer>
#include <QThreadPool>
#include <QWebSocket>
#include <QWebSocketServer>
#include "armsoft.h"
#include "c5searchengine.h"
#include "configini.h"
#include "database.h"
#include "fileversion.h"
#include "logwriter.h"
#include "store_doc_status.h"
#include "store_doc_type.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_storage_item.h"
#include "waiter.h"

QMutex mSocketMutex;

typedef bool (*handler)(const QJsonObject&, QJsonObject&, QString&);
ServerThread::ServerThread(const QString &configPath) :
    QObject(),
    fConfigPath(configPath)
{
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if(db.open("127.0.0.1", "service5", "root", "root5", port)) {
        db.exec("select fkey, fname, concat(fkey, '.', fdb) as fuid from dblist");

        while(db.next()) {
            fDbList.append(db.string("fname"));
            mDatabases[db.string("fuid")] = db.string("fname");
            C5SearchEngine ::init(db.string("fname"), db.string("fuid"));
        }

        C5SearchEngine::init(fDbList);
        LogWriter::write(LogWriterLevel::verbose, "Initialized databases", mDatabases.values().join(","));
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

    if(!port) {
        port = 10002;
    }

    fServer = new QWebSocketServer("BreezeServer", QWebSocketServer::NonSecureMode, this);
    int cores = QThread::idealThreadCount();
    int threads = qMax(4, cores * 2);
    QThreadPool::globalInstance()->setMaxThreadCount(threads);

    LogWriter::write(LogWriterLevel::verbose,
                     "",
                     QString("ThreadPool size: %1 (cores %2)").arg(threads).arg(cores));

    // fServer->setMaxPendingConnections(10000);
    if(!fServer->listen(QHostAddress::Any, port)) {
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
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if(!db.open("127.0.0.1", "service5", "root", "root5", port)) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    db[":fkey"] = jdoc["server_key"].toString();
    db.exec("select * from dblist where fkey=:fkey");
    QJsonArray ja;

    while(db.next()) {
        QJsonObject jt;
        jt["name"] = db.string("fcomment");
        jt["database"] = db.string("fpath");
        jt["settings"] = db.string("fsettings");
        jt["key"] = db.string("fkey");
        jt["db"] = db.string("fdb");
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
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if(!db.open("127.0.0.1", "service5", "root", "root5", port)) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        return repMsg;
    }

    db[":fkey"] = jdoc["server_key"].toString();
    db[":fname"] = jdoc["connection_name"].toString();
    db.exec("select * from dblist where fkey=:fkey and fcomment=:fname");
    QJsonObject jt;

    if(db.next()) {
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

    if(!l.load()) {
        jrep["errorCode"] = 2;
        jrep["errorMessage"] = "Library not found";
        jrep["library"] = libraryPath;
        repMsg =  QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    } else {
        handler h = reinterpret_cast<handler>(l.resolve(jdoc["handler"].toString().toLatin1().data()));

        if(!h) {
            jrep["errorCode"] = 3;
            jrep["errorMessage"] = "Handler not found";
            jrep["handler"] = jdoc["handler"].toString();
            repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        } else {
            QJsonObject jresponse = jrep;
            QString err;

            if(!h(jdoc, jresponse, err)) {
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
    auto dbk = mDatabases.find(jdoc["key"].toString());

    if(dbk == mDatabases.end()) {
        ws->close();
        return;
    }

    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif
    auto fail = [&](int code, const QString &msg) {
        LogWriter::write(LogWriterLevel::errors, "register_socket", msg);
        ws->sendTextMessage(QJsonDocument(QJsonObject{{"errorCode", code}, {"errorMessage", msg}})
                                .toJson(QJsonDocument::Compact));
        ws->close();
    };
    if(!db.open("localhost", dbk.value(), "root", "root5", port)) {
        LogWriter::write(LogWriterLevel::errors, "register_socket", db.lastDbError());

        fail(0, "Code 110");
        return;
    }

    db[":f_username"] = jdoc["username"].toString();
    db[":f_password"] = jdoc["password"].toString();
    db.exec("select f_id from s_user where f_login=:f_username and f_password=md5(:f_password)");

    if(!db.next()) {
        fail(0, "user with used credentials not found");
        return;
    }

    QMutexLocker ml(&mSocketMutex);
    auto it = fSockets.find(ws);

    if(it == fSockets.end()) {
        fail(0, "Code 111");
        return;
    }

    SocketStruct &ss = it.value();

    if(!ss.tenantId.isEmpty()) {
        fail(0, "Code 112");
        return;
    }

    ss.tenantId = dbk.key();
    ss.databaseName = dbk.value();
    ss.userId = db.integer("f_id");
    ws->sendTextMessage(R"({"status":1})");
}

void ServerThread::unregisterSocket(const QJsonObject &jdoc, QWebSocket *ws)
{
    qDebug() << "unregister_socket";
}

QString ServerThread::updateHotelCache(const QJsonObject &jdoc)
{
    QMutexLocker ml(&mSocketMutex);

    for(const SocketStruct &ss : qAsConst(fSockets)) {
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

    if(realIp.isEmpty()) {
        realIp = ws->peerAddress().toString();
    }

    LogWriter::write(LogWriterLevel::verbose, "",
                     QString("New connection from %1, peer name: %2, origin: %3")
                     .arg(realIp)
                     .arg(ws->peerName())
                     .arg(ws->origin()));
    connect(ws, &QWebSocket::textMessageReceived, this, &ServerThread::onTextMessage);
    connect(ws, &QWebSocket::binaryMessageReceived, this, &ServerThread::onBinaryMessage);
    connect(ws, &QWebSocket::disconnected, this, &ServerThread::onDisconnected);
    fSockets[ws] = SocketStruct{ws, "", "", 0};
}

void ServerThread::onDisconnected()
{
    QMutexLocker ml(&mSocketMutex);
    QWebSocket *ws = qobject_cast<QWebSocket*>(sender());

    if(!ws) {
        return;
    }

    auto it = fSockets.find(ws);

    if(it != fSockets.end()) {
        LogWriter::write(
            LogWriterLevel::verbose,
            "",
            QString("disconnected: tenant=%1 user=%2 type=%3")
            .arg(it->databaseName)
            .arg(it->userId)
        );
        fSockets.erase(it);
    } else {
        LogWriter::write(LogWriterLevel::verbose, "", "disconnected: unknown socket");
    }

    ws->deleteLater();
}

void ServerThread::onTextMessage(const QString &msg)
{
    QWebSocket *ws = static_cast<QWebSocket*>(sender());
    QString messageCopy = msg;
    QPointer<QWebSocket> wsCopy(ws);
    auto uuid = Database::uuid();

    if (msg == "ping") {
        wsCopy->sendTextMessage("pong");
    } else {
        auto doc = QJsonDocument::fromJson(messageCopy.toUtf8());
        if (!doc.isObject()) {
            ws->sendTextMessage(R"({"errorCode":1,"errorMessage":"Invalid JSON"})");
            return;
        }
        QJsonObject jdoc = doc.object();
        LogWriterVerbose("REQUEST " + uuid, msg);

        if(jdoc["command"].toString() == "register_socket") {
            registerSocket(jdoc, ws);
            return;
        } else if(jdoc["command"].toString()  == "unregister_socket") {
            unregisterSocket(jdoc, ws);
            return;
        } else {
            QThreadPool::globalInstance()->start(QRunnable::create([this, jdoc, wsCopy, uuid, ws]() {
                if(!wsCopy)
                    return;

                SocketStruct ss;
                {
                    QMutexLocker ml(&mSocketMutex);
                    auto it = fSockets.find(ws);
                    if (it == fSockets.end())
                        return;
                    ss = it.value();
                }

                QJsonObject jresponse;
                QString repMsg;
                handleCommand(ss, jdoc, repMsg);

                if(!wsCopy)
                    return;

                QMetaObject::invokeMethod(wsCopy, [wsCopy, repMsg, uuid]() {
                    if(wsCopy) {
                        LogWriterVerbose("REPLY " + uuid, repMsg);
                        wsCopy->sendTextMessage(repMsg);
                    }
                }, Qt::QueuedConnection);
            }));
        }
    }
}

void ServerThread::onBinaryMessage(const QByteArray &msg)
{
    QWebSocket *ws = static_cast<QWebSocket*>(sender());
    QString messageCopy = msg;
    QPointer<QWebSocket> wsCopy(ws);
    auto uuid = Database::uuid();

    if (msg == QByteArray("ping")) {
        wsCopy->sendBinaryMessage(QByteArray("pong"));
    } else {
        auto doc = QJsonDocument::fromJson(msg);
        if (!doc.isObject()) {
            ws->sendBinaryMessage(R"({"errorCode":1,"errorMessage":"Invalid JSON"})");
            return;
        }
        QJsonObject jdoc = doc.object();
        if(jdoc["command"].toString() == "register_socket") {
            registerSocket(jdoc, ws);
            return;
        } else if(jdoc["command"].toString()  == "unregister_socket") {
            unregisterSocket(jdoc, ws);
            return;
        } else {
            LogWriter::write(LogWriterLevel::verbose, "REQUEST " + uuid, msg);
            QThreadPool::globalInstance()->start(QRunnable::create([this, jdoc, wsCopy, uuid, ws]() {
                if(!wsCopy)
                    return;

                SocketStruct ss;
                {
                    QMutexLocker ml(&mSocketMutex);
                    auto it = fSockets.find(ws);
                    if (it == fSockets.end())
                        return;
                    ss = it.value();
                }

                QJsonObject jresponse;
                QString repMsg;
                handleCommand(ss, jdoc, repMsg);

                if(!wsCopy)
                    return;

                QMetaObject::invokeMethod(wsCopy, [wsCopy, repMsg, uuid]() {
                    if(wsCopy) {
                        LogWriter::write(LogWriterLevel::verbose, "REPLY " + uuid, repMsg);
                        wsCopy->sendBinaryMessage(repMsg.toUtf8());
                    }
                }, Qt::QueuedConnection);
            }));
        }
    }
}

void ServerThread::handleCommand(SocketStruct ws, const QJsonObject &jdoc, QString &repMsg)
{
    QJsonObject jrep;
    QString command = jdoc["command"].toString().toLower();

    if(command == "register_socket" || command == "unregister_socket") {
        jrep["errorCode"] = 400;
        jrep["errorMessage"] = "Invalid command context";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        return;
    }

    bool allowAnonymous =
        command == "who?" ||
        command == "get_db_list" ||
        command == "get_connection";

    if (!allowAnonymous && ws.tenantId.isEmpty()) {
        jrep["errorCode"] = 401;
        jrep["errorMessage"] = "Unauthorized";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriterError(repMsg);
        auto wsSafe = QPointer<QWebSocket>(ws.socket);
        QMetaObject::invokeMethod(
            ws.socket,
            [wsSafe]() {
                if (wsSafe)
                    wsSafe->close();
            },
            Qt::QueuedConnection);
        return;
    }

    if (jdoc.isEmpty()) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Empty or invalid JSON";
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    } else {
        if(command == "who?") {
            jrep["me"] = "Service5";
            jrep["version"] = FileVersion::getVersionString();
            repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        } else if(command == "get_db_list") {
            repMsg = getDbList(jdoc);
        } else if(command == "get_connection") {
            repMsg = getConnection(jdoc);
        } else if(command == "waiter") {
            Waiter w(jdoc);
            repMsg = w.process();
        } else if(command == "hotel_cache_update") {
            repMsg = updateHotelCache(jdoc);
        } else if(command == "search_engine_reload") {
            C5SearchEngine::init(fDbList);
            LogWriter::write(LogWriterLevel::verbose, "Initialized databases", fDbList.join(","));
        } else if(command == "search_text") {
            repMsg = C5SearchEngine::mInstance->search(jdoc);
        } else if(command == SelectorName<StorageItem>::value) {
            repMsg = C5SearchEngine::mInstance->searchStorage(jdoc, ws);
        } else if(command == SelectorName<GoodsItem>::value) {
            repMsg = C5SearchEngine::mInstance->searchGoodsItem(jdoc, ws);
        } else if(command == SelectorName<PartnerItem>::value) {
            repMsg = C5SearchEngine::mInstance->searchPartnerItem(jdoc, ws);
        } else if (command == SelectorName<StoreDocStatusItem>::value) {
            repMsg = C5SearchEngine::mInstance->searchStoreDocStatus(jdoc, ws);
        } else if (command == SelectorName<StoreDocTypeItem>::value) {
            repMsg = C5SearchEngine::mInstance->searchStoreDocType(jdoc, ws);
        } else if (command == "search_partner") {
            repMsg = C5SearchEngine::mInstance->searchPartner(jdoc);
        } else if (command == "search_goods_groups") {
            repMsg = C5SearchEngine::mInstance->searchGoodsGroups(jdoc);
        } else if (command == "search_store") {
            repMsg = C5SearchEngine::mInstance->searchStore(jdoc);
        } else if (command == "search_update_partner_cache") {
            repMsg = C5SearchEngine::mInstance->searchUpdatePartnerCache(jdoc);
        } else if (command == "armsoft") {
            repMsg = armsoft(jdoc);
        } else if (command == "dict_event") {
            repMsg = C5SearchEngine::mInstance->updateDictionary(jdoc, ws);
        } else {
            repMsg = handleDll(jdoc, command);
        }
    }
}
