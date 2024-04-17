#include "ndataprovider.h"
#include "c5filelogwriter.h"
#include "c5config.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>

NDataProvider::NDataProvider(const QString &host, QObject *parent)
    : QObject(parent),
    mHost(host)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    mNetworkAccessManager->setTransferTimeout(30000);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &NDataProvider::queryFinished);
    mTimer = new QElapsedTimer();
}

NDataProvider::~NDataProvider()
{
    delete mTimer;
}

void NDataProvider::getData(const QString &route, const QJsonObject &data)
{
#ifdef QT_DEBUG
    c5log(route);
    c5log(QJsonDocument(data).toJson(QJsonDocument::Compact));
#else
    if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
        c5log(route);
        c5log(QJsonDocument(data).toJson(QJsonDocument::Compact));
    }
#endif
    mTimer->restart();
    emit started();
    QNetworkRequest rq(mHost + route);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["query"] = 3;
    jo["call"] = "sql";
    //jo["sql"] = sql;
    jo["sk"] = "5cfafe13-a886-11ee-ac3e-1078d2d2b808";
    jo["apikey"] = "8eabcee4-f1bc-11ee-8b0f-021eaa527a65-a0d5c784-f1bc-11ee-8b0f-021eaa527a65";
    QStringList keys = data.keys();
    for (const auto &s: keys) {
        jo[s] = data[s];
    }

    mNetworkAccessManager->post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
}

void NDataProvider::queryFinished(QNetworkReply *r)
{

    if (r->error() != QNetworkReply::NoError) {
        emit error(r->errorString() + "\r\n" + r->readAll());
        r->deleteLater();
        return;
    }
    QByteArray ba = r->readAll();
    r->deleteLater();
    emit done(mTimer->elapsed(), ba);
}
