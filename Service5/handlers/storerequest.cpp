#include "storerequest.h"
#include "jsonhandler.h"
#include "storemanager.h"
#include <QJsonObject>
#include <QJsonArray>

StoreRequest::StoreRequest() :
    RequestHandler()
{

}

bool StoreRequest::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    QJsonArray jaGoods;
    QList<StoreRecord> gl;
    StoreManager::queryQty(0, fGoods, gl);
    QMap<QString, double> totalQty;
    for (StoreRecord sr: gl) {
        totalQty[sr.fSku] = totalQty[sr.fSku] + sr.fQty;
    }
    if (fGoods.count() > totalQty.count()) {
        jh["status"] = "partial";
    }
    for (QMap<QString,double>::const_iterator it = totalQty.begin(); it != totalQty.end(); it++) {
        QJsonObject jo;
        jo["sku"] = it.key();
        jo["qty"] = it.value();
        jaGoods.append(jo);
    }
    jh["goods"] = jaGoods;
        jh["status"] = gl.count() == 0 ? "empty" : jh["status"].toString().isEmpty() ? "ok" : jh["status"].toString();

    return setResponse(HTTP_OK, jh.toString());
}

bool StoreRequest::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
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
    if (!dataMap.contains("query")) {
        message += "Query type missing";
    } else {
        fQuery = getData(data, dataMap["query"]);
        if (fQuery != "qty") {
            message += "Invalid query type";
            fQuery = "";
        }
    }
    if (fQuery == "qty") {
        if (!dataMap.contains("goods")) {
            message += "Goods list missing.";
        } else {
            fGoods = QString(getData(data, dataMap["goods"])).split(",", Qt::SkipEmptyParts);
            if (fGoods.isEmpty()) {
                message += "Emtpy goods list";
            }
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
