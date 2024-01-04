#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "database.h"
#include "queryjsonresponse.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void routes(QStringList &r)
{
    r.append("vipclient");
}

bool vipclient(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    Database db;
    JsonHandler jh;
    RequestHandler rh(outdata);
#ifdef ELINA
#ifdef QT_DEBUG
    QString dbname = "store";
#else
    QString dbname = "elina";
#endif
    if (!db.open("18.159.205.66", dbname, "root", "root5")) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery::sqlquery").arg(db.lastDbError()));
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }
#else
    QString configFile = path + "/remotedb.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        jh["message"] = "Server not configured";
        return rh.setInternalServerError(jh.toString());
    }
    if (!db.open(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery::sqlquery").arg(db.lastDbError()));
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }
#endif
    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Applilcation/Json");
    }
    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    QJsonObject jresponse;
    QueryJsonResponse(db, jo, jresponse).getResponse();

    return rh.setResponse(HTTP_OK, QJsonDocument(jresponse).toJson(QJsonDocument::Compact));
}
