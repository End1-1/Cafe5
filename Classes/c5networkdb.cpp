#include "c5networkdb.h"
#include "logwriter.h"
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>

C5NetworkDB::C5NetworkDB(const QString &sql, const QString &host, QObject *parent)
    : QObject{parent},
    fHost(host),
    fSql(sql)
{

}

bool C5NetworkDB::query()
{
    QElapsedTimer t;
    t.start();
    QEventLoop loop;
    QNetworkAccessManager m;
    QNetworkRequest rq(fHost);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m.setProxy(QNetworkProxy::NoProxy);
    // QSslConfiguration sslConf = rq.sslConfiguration();
    // sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    // sslConf.setProtocol(QSsl::AnyProtocol);
    // rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["sk"] = "5cfafe13-a886-11ee-ac3e-1078d2d2b808";
    jo["call"] = "sql";
    jo["sql"] = fSql;
    auto *r = m.post(rq, QJsonDocument(jo).toJson());
    r->connect(r, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (r->error() != QNetworkReply::NoError) {
        fJsonOut["status"] = 0;
        fJsonOut["data"] = r->errorString();
        LogWriter::write(LogWriterLevel::verbose, "",
                         QString("Elapsed: %1, query: %2, error: %3")
                             .arg(QString::number(t.elapsed()), fSql, fJsonOut["data"].toString()));
        return false;
    }
    QByteArray ba = r->readAll();
    QJsonParseError err;
    fJsonOut = QJsonDocument::fromJson(ba, &err).object();
    if (err.error != QJsonParseError::NoError) {
        fJsonOut["status"] = 0;
        fJsonOut["data"] = err.errorString();
        LogWriter::write(LogWriterLevel::errors, "",
                         QString("Elapsed: %1, query: %2, error: %3")
                             .arg(QString::number(t.elapsed()), fSql, QString(ba) + "====" + err.errorString()));
        return false;
    }
    QJsonArray ja = fJsonOut["data"].toObject()["types"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        switch (ja[i].toInt()) {
        case 3:
            ja[i] = QVariant::Int;
            break;
        case 246:
            ja[i] = QVariant::Double;
            break;
        case 245:
            ja[i] = QVariant::String;
            break;
        default:
            ja[i] = QVariant::String;
            break;
        }
    }
    fJsonOut["data"].toObject()["types"] = ja;
#ifdef QT_DEBUG
    qDebug() << t.elapsed() << fSql;
#endif
}

bool C5NetworkDB::query(const QString &sql)
{
    fSql = sql;
    return query();
}
