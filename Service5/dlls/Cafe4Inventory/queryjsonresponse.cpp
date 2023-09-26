#include "queryjsonresponse.h"
#include <QJsonArray>

#define rCafeList 1
#define rStoreList 2
#define rGetBaseItems 3
#define rAmtStorageList 4

QueryJsonResponse::QueryJsonResponse(QSettings &s, const QJsonObject &ji, QJsonObject &jo) :
    fSettings(s),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["kStatus"] = 1;
    fJsonOut["kData"] = "";

    switch (fJsonIn["request"].toInt()) {
    case rCafeList:
        getCafeList();
        break;
    case rStoreList:
        getStoreList();
        break;
    case rGetBaseItems:
        getBaseItems();
        break;
    case rAmtStorageList:
        getAmtStorageList();
        break;
    default:        
        fJsonOut["kData"] = "Unknown query";
        fJsonOut["kStatus"] = 4;
        break;
    }
}

void QueryJsonResponse::dbToArray(QJsonArray &ja, Database  &db)
{
    while (db.next()) {
        QJsonObject jo;
        for (int i = 0; i < db.columnCount(); i++){
            switch (db.value(i).type()) {
            case QVariant::Int:
            case QVariant::Char:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                jo[db.columnName(i)] = db.integer(i);
                break;
            case QVariant::Double:
                jo[db.columnName(i)] = db.doubleValue(i);
                break;
            default:
                jo[db.columnName(i)] = db.string(i);
                break;
            }
        }
        ja.append(jo);
    }
}

void QueryJsonResponse::getCafeList()
{
    fSettings.beginGroup("cafelist");
    QStringList keys = fSettings.allKeys();
    QJsonArray ja;
    for (const QString &s: qAsConst(keys)) {
        QJsonObject jo;
        jo["id"] = s;
        jo["name"] = fSettings.value(s).toString();
        ja.append(jo);
    }
    fSettings.endGroup();
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getStoreList()
{
    fSettings.beginGroup("storelist");
    QStringList keys = fSettings.allKeys();
    QJsonArray ja;
    for (const QString &s: qAsConst(keys)) {
        QJsonObject jo;
        jo["id"] = s;
        jo["name"] = fSettings.value(s).toString();
        ja.append(jo);
    }
    fSettings.endGroup();
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getBaseItems()
{
    fSettings.beginGroup("asjazzve");
    Database db("QODBC3");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value("query/baseitem").toString()
            .replace("%date%", QDate::currentDate().toString("yyyy-MM-dd"))
            .replace("%department%", fJsonIn["store"].toString());
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getAmtStorageList()
{
    fSettings.beginGroup("asjazzve");
    Database db("QODBC3");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value("query/basestorages").toString();
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}


