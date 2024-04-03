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

void NDataProvider::getData(const QString &route)
{
#ifdef QT_DEBUG
    C5FileLogWriter::write(route);
#else
    if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
        C5FileLogWriter::write(route);
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
    mNetworkAccessManager->post(rq, QJsonDocument(jo).toJson());

//    QByteArray ba = r->readAll();
//    quint64 elapsed = t.elapsed();
//    jo = QJsonDocument::fromJson(ba).object();
//    if (jo["status"].toInt() == 0) {
//        logEvent(ba);
//        return false;
//    }

//        return true;
//    }
//    jo = jo["data"].toObject();
//    QJsonArray ja = jo["columns"].toObject()["column_index_name"].toArray();
//    fNameColumnMap.clear();
//    for (int i = 0; i < ja.size(); i++) {
//        const QJsonObject &jc = ja[i].toObject();
//        fNameColumnMap[jc["name"].toString()] = jc["value"].toInt();
//    }
//    ja = jo["columns"].toObject()["column_name_index"].toArray();
//    QJsonArray jtype = jo["types"].toArray();
//    // fNameColumnMap.clear();
//    // for (int i = 0; i < ja.size(); i++) {
//    //     const QJsonObject &jc = ja[i].toObject();
//    //     fNameColumnMap[jc["name"].toString()] = jc["value"].toInt();
//    // }
//    ja = jo["data"].toArray();
//    fDbRows.clear();
//    for (int i = 0; i < ja.size(); i++) {
//        QList<QVariant> r;
//        QJsonArray jar = ja[i].toArray();
//        for (int j = 0; j < jar.size(); j++) {
//            switch (jtype[j].toInt()) {
//            case 3:
//                r.append(jar[j].toInt());
//                break;
//            case 246:
//                r.append(jar[j].toDouble());
//                break;
//            case 10:
//                r.append(QDate::fromString(jar[j].toString(), FORMAT_DATE_TO_STR_MYSQL));
//                break;
//            case 11:
//                r.append(QTime::fromString(jar[j].toString()));
//                break;
//            case 7:
//                r.append(QDateTime::fromString(jar[j].toString()));
//                break;
//            default:
//                r.append(jar[j].toString());
//                break;
//            }

//        }
//        fDbRows.append(r);
//    }

//    return true;
}

void NDataProvider::queryFinished(QNetworkReply *r)
{

    if (r->error() != QNetworkReply::NoError) {
        emit error(r->errorString());
        r->deleteLater();
        return;
    }
    QByteArray ba = r->readAll();
    r->deleteLater();
    emit done(mTimer->elapsed(), ba);
}
