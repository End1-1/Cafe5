#include "c5scheduler.h"
#include "server5settings.h"
#include "c5database.h"
#include "c5databasesync.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonParseError>

c5scheduler::c5scheduler(QObject *parent) : QObject(parent)
{
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
#ifdef QT_DEBUG
    fTimer.setInterval(5000 * __s.value("updateinterval").toInt());
#else
    fTimer.setInterval(1000 * 60 * __s.value("updateinterval").toInt());
#endif
    fTimer.start();
    fRun = false;
    C5DatabaseSync *db = new C5DatabaseSync(this);
    db->start();
}

void c5scheduler::timeout()
{
    if (fRun) {
        return;
    }
    fRun = true;
    if (__s.value("url").toString().isEmpty()) {
        uploadStatistic();
    }
}

void c5scheduler::replyFinished(QNetworkReply *r)
{
    if (r->error() == QNetworkReply::NoError) {
        qDebug() << r->readAll();
    } else {
        qDebug() << r->errorString();
    }
    r->deleteLater();
    fRun = false;
}

void c5scheduler::uploadStatistic()
{
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(__s.value("queries").toString().toUtf8(), &err);
    if (err.error != err.NoError) {
        return;
    }
    QJsonObject jo = jdoc.object();
    QJsonArray ja = jo["reports"].toArray();
    C5Database db(__s.value("dbhost").toString(), __s.value("dbschema").toString(), __s.value("dbuser").toString(), __s.value("dbpassword").toString());
    QJsonArray jaup;
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject jr = ja.at(i).toObject();
        QString sql = jr["sql"].toString();
        QJsonObject jup;
        jup["name"] = jr["name"].toString();
        jup["date1"] = QDate::currentDate().addDays(-1 * __s.value("updatedays").toInt()).toString("yyyy-MM-dd");
        jup["date2"] = QDate::currentDate().toString("yyyy-MM-dd");
        db[":date1"] = QDate::currentDate().addDays(-1 * __s.value("updatedays").toInt());
        db[":date2"] = QDate::currentDate();
        db.exec(sql);
        QJsonArray jdata;
        while (db.nextRow()) {
            QJsonObject jrow;
            for (int j = 0; j < db.columnCount(); j++) {
                jrow[db.columnName(j)] = db.getString(j);
            }
            jdata.append(jrow);
        }
        jup["data"] = jdata;
        jaup.append(jup);
    }
    db.commit();
    jdoc = QJsonDocument(jaup);

    QNetworkAccessManager * manager = new QNetworkAccessManager(this);
    QUrl url(__s.value("url").toString());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("data", jdoc.toJson(QJsonDocument::Compact));
    params.addQueryItem("reqpass", __s.value("urlsecret").toString());
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply*)));
    manager->post(request, params.query().toUtf8());
}
