#include "queryjsonresponse.h"
#include <QJsonArray>

QueryJsonResponse::QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["kStatus"] = 1;
    fJsonOut["kData"] = "";

    if (fJsonIn["queries"].toArray().isEmpty()) {
        fJsonOut["kStatus"] = 4;
        fJsonOut["kData"] = "Unknown query";
        return;
    }

    fDb.startTransaction();
    for (int i = 0; i < fJsonIn["queries"].toArray().size(); i++) {
        const QJsonObject &jo = fJsonIn["queries"][i].toObject();
        fDb[":f_name"] = jo["name"].toString();
        fDb.exec("select * from remotedb_sql where f_name=:f_name");
        if (fDb.next() == false) {
            fDb.rollback();
            fJsonOut["kData"] = QString("Unknown query with name '%1'").arg(jo["name"].toString());
            fJsonOut["kStatus"] = 4;
            return;
        }
        QStringList query = fDb.string("f_sql").split(";", Qt::SkipEmptyParts);
        for (const QString &q: query) {
            for (int j = 0; j < jo["params"].toArray().size(); j++) {
                const QJsonObject &keyValue = jo["params"][j].toObject();
                for (const QString &key: keyValue.keys()) {
                    switch (keyValue[key].type()) {
                    case QJsonValue::Double:
                        fDb[":" + key] = keyValue[key].toDouble();
                        break;
                    default:
                        fDb[":" + key] = keyValue[key].toString();
                        break;
                    }
                }
            }
            if (fDb.exec(q) == false) {
                fJsonOut["kData"] = fDb.lastDbError();
                fJsonOut["kStatus"] = 4;
                fDb.rollback();
                return;
            }
        }
    }
    QJsonArray ja;
    dbToArray(ja);
    fDb.commit();
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::dbToArray(QJsonArray &ja)
{
    while (fDb.next()) {
        QJsonObject jo;
        for (int i = 0; i < fDb.columnCount(); i++){
            switch (fDb.value(i).type()) {
            case QVariant::Int:
            case QVariant::Char:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                jo[fDb.columnName(i)] = fDb.integer(i);
                break;
            case QVariant::Double:
                jo[fDb.columnName(i)] = fDb.doubleValue(i);
                break;
            default:
                jo[fDb.columnName(i)] = fDb.string(i);
                break;
            }
        }
        ja.append(jo);
    }
}
