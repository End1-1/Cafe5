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
    if (fQuery == "qty") {
        QJsonArray jaGoods;
        QMap<QString, double> gl;
        StoreManager::queryQty(0, fGoods, gl);

        if (fGoods.count() > gl.count()) {
            jh["status"] = "partial";
        }
        for (QMap<QString,double>::const_iterator it = gl.begin(); it != gl.end(); it++) {
            QJsonObject jo;
            jo["sku"] = it.key();
            jo["qty"] = it.value();
            jaGoods.append(jo);
        }
        jh["goods"] = jaGoods;
        jh["status"] = gl.count() == 0 ? "empty" : jh["status"].toString().isEmpty() ? "ok" : jh["status"].toString();
    } else if (fQuery == "qty2") {
        QJsonArray jaStore;
        QMap<int, QMap<QString, double> > gl;
        StoreManager::queryQty(fGoods, gl);
        for (QMap<int, QMap<QString,double> >::const_iterator it = gl.begin(); it != gl.end(); it++) {
            QJsonArray jaGoods;
            for (QMap<QString, double>::const_iterator it2 = it.value().begin(); it2 != it.value().end(); it2++) {
                QJsonObject jo;
                jo["sku"] = it2.key();
                jo["qty"] = it2.value();
                jaGoods.append(jo);
            }
            QJsonObject js;
            js["store"] = it.key();
            js["goods"] = jaGoods;
            jaStore.append(js);
        }
        jh["data"] = jaStore;
        jh["status"] = gl.count() == 0 ? "empty" : jh["status"].toString().isEmpty() ? "ok" : jh["status"].toString();
    }
    return setResponse(HTTP_OK, jh.toString());
}

bool StoreRequest::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    QStringList queryTypes;
    queryTypes.append("qty");
    queryTypes.append("qty2");

    if (!dataMap.contains("key")) {
        message += "Product key missing.";
    } else {
        QString key = getData(data, dataMap["key"]);
        if (key != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    }
    fQuery.clear();
    if (!dataMap.contains("query")) {
        message += "Query type missing";
    } else {
        for (const QString &qt: queryTypes) {
            if (qt == getData(data, dataMap["query"])) {
                fQuery = getData(data, dataMap["query"]);
                break;
            }
        }
        if (fQuery.isEmpty()) {
            message += "Invalid query type";
            fQuery = "";
        }
    }

        if (!dataMap.contains("goods")) {
            message += "Goods list missing.";
        } else {
            fGoods = QString(getData(data, dataMap["goods"])).split(",", Qt::SkipEmptyParts);
            if (fGoods.isEmpty()) {
                message += "Emtpy goods list";
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
