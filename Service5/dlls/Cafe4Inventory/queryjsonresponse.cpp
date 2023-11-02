#include "queryjsonresponse.h"
#include <QJsonArray>

#define rCafeList 1
#define rStoreList 2
#define rGetBaseItems 3
#define rAmtStorageList 4
#define rAsStorageList 5
#define rGetStorageItem 6
#define rOpenInventoryDoc 7
#define rUpdateInventoryDocItem 8
#define rGetGoodsQrAndWeight 9
#define rGetGoodsNames 10
#define rSaveGoodsQrName 11
#define rGetConfig 12
#define rGetAragamash 13
#define rGetAllTogether 14

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
    case rAsStorageList:
        getAsStorageList();
        break;
    case rGetStorageItem:
        getStorageItems();
        break;
    case rOpenInventoryDoc:
        openInventoryDoc();
        break;
    case rUpdateInventoryDocItem:
        updateInventoryDocItem();
        break;
    case rGetGoodsQrAndWeight:
        getGoodsQrAndWeight();
        break;
    case rGetGoodsNames:
        getGoodsNames();
        break;
    case rSaveGoodsQrName:
        saveGoodsQrName();
        break;
    case rGetConfig:
        getConfig();
        break;
    case rGetAragamash:
        getAragamash();
        break;
    case rGetAllTogether:
        getAllTogether();
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

QString QueryJsonResponse::fbDb()
{
    return fJsonIn["workerDb"].toString();
}

void QueryJsonResponse::returnQueryResult(const QString &queryName)
{
    fSettings.beginGroup(fbDb());
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value(QString("query/%1").arg(queryName)).toString();
    if (!db.exec(query)) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
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
    QString where;
    if (fJsonIn["store"].toString().isEmpty() == false) {
        where += QString("AND OPER.fDEPARTIN = '%1' ").arg(fJsonIn["store"].toString());
    }
    QString query = fSettings.value("query/baseitem").toString()
            .replace("%date%", QDate::currentDate().toString("yyyy-MM-dd"))
            .replace("%where%", where);
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    if (fJsonIn["item"].toString().isEmpty() == false) {
        QJsonArray jaTemp;
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jo = ja.at(i).toObject();
            if (jo["finvnumcode"].toString() == fJsonIn["item"].toString()) {
                jaTemp.append(jo);
            }
        }
        ja = jaTemp;
    }
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

void QueryJsonResponse::getAsStorageList()
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
    QString query = fSettings.value("query/storages").toString();
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getStorageItems()
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
    QString where;
    if (fJsonIn["store"].toString().isEmpty() == false){
        where += QString("AND H.fSTORAGE ='%1'").arg(fJsonIn["store"].toString());
    }
    QString query = fSettings.value("query/storageitem").toString()
            .replace("%date%", QDate::currentDate().toString("yyyy-MM-dd"))
            .replace("%where%", where);
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    if (fJsonIn["item"].toString().isEmpty() == false) {
        QJsonArray jaTemp;
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jo = ja.at(i).toObject();
            if (jo["fmtcode"].toString() == fJsonIn["item"].toString()) {
                jaTemp.append(jo);
            }
        }
        ja = jaTemp;
    }
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::openInventoryDoc()
{
    fSettings.beginGroup(fbDb());
    int docid = fSettings.value("bardocid").toInt();
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value("query/openinvdoc").toString();
    db[":doc_id"] = docid;
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::updateInventoryDocItem()
{
    fSettings.beginGroup(fbDb());
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value("query/updateinvdocitem").toString();
    db[":id"] = fJsonIn["id"].toInt();
    db[":qty"] = fJsonIn["qty"].toDouble() - fJsonIn["tara"].toDouble();
    db.exec(query);
    QJsonObject jo;
    jo["id"] = fJsonIn["id"].toInt();
    jo["goods_id"] = fJsonIn["goods_id"].toInt();
    jo["qty"] = fJsonIn["qty"].toDouble();
    jo["tara"] = fJsonIn["tara"].toDouble();
    jo["name"] = fJsonIn["name"].toString();
    jo["qr"] = fJsonIn["qr"].toString();
    fJsonOut["kData"] = jo;
}

void QueryJsonResponse::getGoodsQrAndWeight()
{
    fSettings.beginGroup(fbDb());
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value("query/qrweightlist").toString();
    QString where;
    if (fJsonIn["qr"].toString().isEmpty() == false) {
        where += QString(" where qw.scancode='%1'").arg(fJsonIn["qr"].toString());
    }
    if (!db.exec(query + where)) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
    if (ja.size() > 0) {
        fJsonOut["found"] = 1;
    }
}

void QueryJsonResponse::getGoodsNames()
{
    fSettings.beginGroup(fbDb());
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fSettings.endGroup();
    QString query = fSettings.value(QString("query/%1").arg("goodsnames")).toString();
    QString where;
    if (fJsonIn["filter"].toString().isEmpty() == false) {
        where += QString(" where lower(name) like '%%1%' ").arg(fJsonIn["filter"].toString().toLower());
    }
    if (!db.exec(query.replace("%where%", where))) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::saveGoodsQrName()
{
    fSettings.beginGroup(fbDb());
    Database db("QIBASE");
    if (!db.open(fSettings.value("host").toString(),
                 fSettings.value("db").toString(),
                 fSettings.value("user").toString(),
                 fSettings.value("password").toString())) {
        fJsonOut["kData"] = db.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    QString qr = fJsonIn["qr"].toString();
    if (qr.isEmpty()){
        fJsonOut["kData"] = "QR code is empty";
        fJsonOut["kStatus"] = 4;
        return;
    }
    if (fJsonIn["goods_id"].toInt() == 0) {
        fJsonOut["kData"] = "Goods must be defined";
        fJsonOut["kStatus"] = 4;
        return;
    }
    int id = fJsonIn["id"].toInt();
    db[":scancode"] = qr;
    db.exec("select * from food_scancode where scancode=:scancode");
    if (db.next()) {
        db[":food_id"] = fJsonIn["goods_id"].toInt();
        db[":weight"] = fJsonIn["qty"].toDouble();
        db.update("food_scancode", "id", id);
    } else {
        db[":food_id"] = fJsonIn["goods_id"].toInt();
        db[":scancode"] = qr;
        db[":weight"] = fJsonIn["qty"].toDouble();
        db.insert("food_scancode");
        db[":scancode"] = qr;
        db.exec("select * from food_scancode where scancode=:scancode");
        if (db.next()) {
           id = db.integer("id");
        }
    }
    fJsonOut["id"] = id;
    fJsonOut["goods_id"] = fJsonIn["goods_id"];
    fJsonOut["qty"] = fJsonIn["qty"];
    fJsonOut["name"] = fJsonIn["name"];
    fJsonOut["qr"] = fJsonIn["qr"];
    fJsonOut["comment"] = fJsonIn["comment"];
}

void QueryJsonResponse::getConfig()
{
    fSettings.beginGroup(QString("config_%1").arg(fJsonIn["config"].toString()));
    QStringList keys = fSettings.allKeys();
    QJsonArray ja;
    for (const QString &s: qAsConst(keys)) {
        QJsonObject jo;
        jo[s] = fSettings.value(s).toString();
        ja.append(jo);
    }
    fSettings.endGroup();
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getAragamash()
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
    QString query = fSettings.value("query/aragamashitem").toString();
    QString where;
    if (fJsonIn["item"].toString().isEmpty() == false) {
        where += QString(" and M.fMTCODE='%1'").arg(fJsonIn["item"].toString());
    }
    if (fJsonIn["store"].toString().isEmpty() == false){
        where += QString(" and H.fFADEPARTMENT='%1'").arg(fJsonIn["store"].toString());
    }
    query.replace("%where%", where);
    db.exec(query);
    QJsonArray ja;
    dbToArray(ja, db);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::getAllTogether()
{

}


