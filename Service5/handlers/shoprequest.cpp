#include "shoprequest.h"
#include "jsonhandler.h"
#include "databaseconnectionmanager.h"
#include "shopmanager.h"
#include <QJsonDocument>
#include <QMutex>

QMap<int, int> ShopRequest::fShopCounters;
QMutex fMutex;

ShopRequest::ShopRequest() :
    RequestHandler()
{
    if (fShopCounters.isEmpty()) {
        fShopCounters.insert(10, 1);
    }
}

bool ShopRequest::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db;
    if (!DatabaseConnectionManager::openDatabase(ShopManager::databaseName(), db, jh)) {
        return setInternalServerError(jh.toString());
    }
    QString uuid, userid, err;
    if (!genOrderId(db, uuid, userid, err)) {
        return setInternalServerError(err);
    }
    jh["status"] = "OK";
    jh["uuid"] = uuid;
    jh["order_id"] = userid;
    return setResponse(HTTP_OK, jh.toString());
}

bool ShopRequest::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    switch (fContentType) {
    case ContentType::ApplilcationJson:
        return validateApplicationJson(data, dataMap);
    default:
        return validateMultipartFormData(data, dataMap);
    }
    return false;
}

bool ShopRequest::genOrderId(Database &db, QString &uuid, QString &userid, QString &err)
{
    QMutexLocker ml(&fMutex);
    uuid = db.uuid();
    fShopCounters[10] = fShopCounters[10] + 1;
    userid = QString("%1-%2").arg("O").arg(fShopCounters[10]);
    return err.isEmpty();
}

bool ShopRequest::validateMultipartFormData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    if (!dataMap.contains("key")) {
        message += "Product key missing.";
    } else {
        QString key = getData(data, dataMap["key"]);
        if (key != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    }
    if (!dataMap.contains("order")) {
        message += "Order missing.";
    } else {
        QJsonParseError jerror;
        QJsonDocument jdoc = QJsonDocument::fromJson(getData(data, dataMap["order"]), &jerror);
        if (jerror.error == QJsonParseError::NoError) {
            fOrderJson = jdoc.object();
            if (fOrderJson.isEmpty()) {
                message += "Emtpy order";
            }
        } else {
            message += jerror.errorString() + "-----" + getData(data, dataMap["order"]);
        }
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        fHttpHeader.setResponseCode(HTTP_DATA_VALIDATION_ERROR);
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    }
    return message.isEmpty();
}

bool ShopRequest::validateApplicationJson(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(getData(data, dataMap["json"]), &jerror);
    QJsonObject jo = jdoc.object();
    if (jerror.error == QJsonParseError::NoError) {
        fOrderJson = jo["order"].toObject();
        if (fOrderJson.isEmpty()) {
            message += "Emtpy order";
        }
        if (jo["key"].toString() != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    } else {
        message += jerror.errorString() + "-----" + getData(data, dataMap["json"]);
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        fHttpHeader.setResponseCode(HTTP_DATA_VALIDATION_ERROR);
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    }
    return message.isEmpty();
}
