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
    r.append("skuqty");
}

bool skuqty(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    Database db;
    JsonHandler jh;
    RequestHandler rh(outdata);
    QString configFile = path + "/skuqty.ini";
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

    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Application/Json");
    }

    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    if (jo["key"].toString() != "asdf7f8kk49888d!!jjdjmskkak98983mj???m") {
        return rh.setResponse(HTTP_FORBIDDEN, "Access denied");
    }

    db.exec("SELECT g.f_scancode AS sku, gp.f_price1 AS price, g.f_name as name "
            "FROM c_goods g "
            "LEFT join c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1 "
            "WHERE LENGTH(g.f_scancode)>0 AND gp.f_price1 >0");
    QJsonArray ja;
    while (db.next()){
        QJsonObject js;
        js["sku"] = db.string("sku");
        js["price"] = db.doubleValue("price");
        js["name"] = db.string("name");
        ja.append(js);
    }

    return rh.setResponse(HTTP_OK, QJsonDocument(ja).toJson(QJsonDocument::Compact));
}
