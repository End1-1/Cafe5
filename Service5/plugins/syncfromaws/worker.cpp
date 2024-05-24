#include "worker.h"
#include "logwriter.h"
#include "ndataprovider.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>

WaiterClientHandler::WaiterClientHandler(QObject *parent) :
    QObject(parent),
    fTaskCounter(0)
{
    mNetworkAccessManager = new QNetworkAccessManager();
    mTaskRunning = false;
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &WaiterClientHandler::finished);
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
    mNetworkAccessManager->deleteLater();
    qDebug() << "~WaiterClientHandler()";
}

void WaiterClientHandler::run(RawMessage *rm, const QByteArray &in)
{
    emit handleData(rm, in);
}

void WaiterClientHandler::removeSocket(SslSocket *s)
{
    emit removeSocketHolder(s);
}

void WaiterClientHandler::clearTask()
{
    auto *dp = new NDataProvider(mLocal["database"].toString(), this);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::taskFinish);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/clear.php", { });
}

void WaiterClientHandler::tasks()
{
    fTaskCounter++;
    if (mTaskRunning) {
        return;
    }
    mTaskRunning = true;
    LogWriter::write(LogWriterLevel::verbose, "", "Syncfromaws started");
    QString host = QString("https://%1:10002/%2").arg("127.0.0.1", "office");
    QNetworkRequest rq(host);
    mNetworkAccessManager->setTransferTimeout(10000);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["key"] = "asdf7fa8kk49888d!!jjdjmskkak98983mj???m";
    mNetworkAccessManager->post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
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

void WaiterClientHandler::finished(QNetworkReply *r)
{
    if (r->error() != QNetworkReply::NoError) {
        LogWriter::write(LogWriterLevel::errors, "", r->errorString());
        r->deleteLater();
        mTaskRunning = false;
        return;
    }
    QJsonParseError jerr;
    QByteArray ba = r->readAll();
    r->deleteLater();
    mServers = QJsonDocument::fromJson(ba, &jerr).array();
    if (jerr.error != QJsonParseError::NoError) {
        LogWriter::write(LogWriterLevel::errors, "", jerr.errorString());
        LogWriter::write(LogWriterLevel::errors, "", ba);
        mTaskRunning = false;
        return;
    }
    qDebug() << mServers;
    mLocal = QJsonObject();
    mRemote = QJsonObject();
    for (int i = 0; i < mServers.size(); i++) {
        QJsonObject jo = mServers.at(i).toObject();
        if (jo["local"].toInt() == 0) {
            mRemote = jo;
        } else {
            mLocal = jo;
        }
    }
    auto *dp = new NDataProvider(mLocal["database"].toString(), this);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::getLastTime);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    dp->getData("/engine/sync/get-last-time.php", {});
}

void WaiterClientHandler::getSyncError(const QString &err)
{
    sender()->deleteLater();
    auto *nd = static_cast<NDataProvider *>(sender());
    LogWriter::write(LogWriterLevel::errors, "", err);
    nd->deleteLater();
    mTaskRunning = false;
}

void WaiterClientHandler::getSyncObject(const QJsonObject &ba)
{
    sender()->deleteLater();
    QJsonObject jo = ba;
    taskFinish(ba);
}

void WaiterClientHandler::getLastTime(const QJsonObject &ba)
{
    sender()->deleteLater();
#ifdef QT_DEBUG
    LogWriter::write(LogWriterLevel::verbose, "getLastTime", QJsonDocument(ba).toJson());
#endif
    QJsonObject jo = ba["data"].toObject();
    auto *dp = new NDataProvider(mLocal["database"].toString(), this);
    dp->changeTimeout(1000 * 60 * 10);
    connect(dp, &NDataProvider::done, this, &WaiterClientHandler::getSyncObject);
    connect(dp, &NDataProvider::error, this, &WaiterClientHandler::getSyncError);
    mSyncArrary = jo["result"].toArray();
    dp->getData("/engine/sync/get-sync-object.php", {
        {"server", mRemote["database"].toString()}
    });
}
