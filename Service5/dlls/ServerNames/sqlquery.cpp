#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "database.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void routes(QStringList &r)
{
    r.append("servernames");
}

bool servernames(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    Database db;
    JsonHandler jh;
    RequestHandler rh(outdata);
//    QString configFile = path + "/skuqty.ini";
//    if (!QFile::exists(configFile)) {
//        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
//        jh["message"] = "Server not configured";
//        return rh.setInternalServerError(jh.toString());
//    }
//    if (!db.open(configFile)) {
//        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery::sqlquery").arg(db.lastDbError()));
//        jh["message"] = "Cannot connect to database";
//        return rh.setInternalServerError(jh.toString());
//    }

    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Application/Json");
    }

    qDebug() << indata;
    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    qDebug() << jo["key"].toString();
    if (jo["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        return rh.setResponse(HTTP_FORBIDDEN, "Access denied");
    }

//    db.exec("SELECT g.f_scancode AS sku, gp.f_price1 AS price, g.f_name as name, "
//            "gp.f_price1 as price1, coalesce(gp.f_price1disc, 0) as price2 "
//            "FROM c_goods g "
//            "LEFT join c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1 "
//            "WHERE LENGTH(g.f_scancode)>0 AND gp.f_price1 >0");
//    QJsonArray ja;
//    while (db.next()){
//        QJsonObject js;
//        js["sku"] = db.string("sku");
//        js["price"] = db.doubleValue("price");
//        js["price1"] = db.doubleValue("price1");
//        js["price2"] = db.doubleValue("price2");
//        js["name"] = db.string("name");
//        ja.append(js);
//    }

#ifdef REMOTE_VALSH
    QJsonArray ja;
    jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "valsh.picassocloud.com/info.php";
    jo["database"] = "https://valsh.picassocloud.com/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    ja.append(jo);
#ifdef QT_DEBUG
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    ja.append(jo);
#else
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "192.168.88.5/engine/info.php";
    jo["database"] = "https://192.168.88.5/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    ja.append(jo);
#endif
#endif

    return rh.setResponse(HTTP_OK, QJsonDocument(ja).toJson(QJsonDocument::Compact));
}
