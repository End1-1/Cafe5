#include "worker.h"
#include "logwriter.h"
#include "ndataprovider.h"
#include <QJsonParseError>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>

#ifdef VALSHIN

    #define local "https://127.0.0.1"
    #define remote "https://valsh.picassocloud.com"

    #ifdef QT_DEBUG
        QString db = "valshinw";
    #else
        QString db = "cafe5";
    #endif

#endif

#ifdef ELINA

    #define local "https://127.0.0.1"
    #define remote "https://aws.elina.am"

    #ifdef QT_DEBUG
        QString db = "elina";
    #else
        QString db = "store";
    #endif
#endif

WaiterClientHandler::WaiterClientHandler(QObject *parent) :
    QObject(parent),
    fTaskCounter(0)
{
    mTaskRunning = false;
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WaiterClientHandler::tasks);
#ifdef QT_DEBUG
    timer->start(10000);
#else
    timer->start(120000);
#endif
}

WaiterClientHandler::~WaiterClientHandler()
{
}

void WaiterClientHandler::run(RawMessage *rm, const QByteArray &in)
{
    emit handleData(rm, in);
}

void WaiterClientHandler::removeSocket(SslSocket *s)
{
    emit removeSocketHolder(s);
}

void WaiterClientHandler::getLastId()
{
    LogWriter::write(LogWriterLevel::verbose, "", "Syncfromaws started");
    NDataProvider::mHost = local;
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::getLastIDResponse);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/get-last-id.php", QJsonObject {{"db", db}});
}

void WaiterClientHandler::clearTask()
{
    NDataProvider::mHost = local;
    auto *dp = new NDataProvider(this);
    dp->changeTimeout(1000 * 60 * 60);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::taskFinish);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/clear.php", QJsonObject{{"db", db} });
}

void WaiterClientHandler::tasks()
{
    fTaskCounter++;
    if (mTaskRunning) {
        return;
    }
    mTaskRunning = true;
    getLastId();
}

void WaiterClientHandler::getLastIDResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    jo = jo["result"].toObject();
    int lastid = jo["f_syncin"].toInt();
    NDataProvider::mHost = remote;
    LogWriter::write(LogWriterLevel::verbose, "getLastIDResponse", QString::number(lastid));
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::getDataResponse);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/get-data.php", QJsonObject {{"db", db}, {"id", lastid}});
}

void WaiterClientHandler::getDataResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    if (jo["empty"].toBool()) {
        LogWriter::write(LogWriterLevel::verbose, "getDataResponse", "empty");
        clearTask();
        return;
    }
    QJsonArray ja = jdoc["data"].toArray();
    LogWriter::write(LogWriterLevel::verbose, "getDataResponse started", QString::number(ja.size()));
    NDataProvider::mHost = local;
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::updateLocalResponse);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/update-local-data.php", QJsonObject {{"db", db}, {"data", ja}, {"id", jo["id"].toInt()}});
}

void WaiterClientHandler::updateLocalResponse(const QJsonObject &jdoc)
{
    LogWriter::write(LogWriterLevel::verbose, "updateLocalResponse", "");
    getLastId();
}

void WaiterClientHandler::taskFinish(const QJsonObject &ba)
{
    mTaskRunning = false;
    sender()->deleteLater();
    QJsonObject jo = ba;
#ifdef QT_DEBUG
    LogWriter::write(LogWriterLevel::verbose, "getSyncObjects", QJsonDocument(ba).toJson());
#endif
    LogWriter::write(LogWriterLevel::verbose, "Syncfromaws finished", QJsonDocument(ba).toJson());
}

void WaiterClientHandler::broadcastForClients(QByteArray data)
{
    qDebug() << "Broadcast for clients";
    emit sendToAllClients(data);
}

void WaiterClientHandler::removeMeFromConnectionList(QString uuid)
{
}

void WaiterClientHandler::getSyncError(const QString &err)
{
    sender()->deleteLater();
    auto *nd = static_cast<NDataProvider *>(sender());
    LogWriter::write(LogWriterLevel::errors, "", err);
    nd->deleteLater();
    mTaskRunning = false;
}
