#include "jsonreponse.h"
#include "actions.h"
#include "locale.h"

JsonReponse::JsonReponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void JsonReponse::getResponse()
{
    qDebug() << fJsonIn;
    if (fJsonIn["pkAction"].toInt() != hqRegisterDevice) {
        if (!checkAPIandFCMToken()) {
            return;
        }
    }
    switch (fJsonIn["pkAction"] .toInt()) {
    case hqRegisterDevice:
        registerDevice();
        break;
    case hqLogin:
        login();
        break;
    case hqDownloadData:
        downloadData();
        break;
    case hqCheckPassHash:
        checkHashOfPass();
        break;
    case hqStock:
        stock();
        break;
    default:
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = QString("%1 %2").arg(lkUnknownProtocol, fJsonIn["pkAction"].toString());
        break;
    }
}

void JsonReponse::dbToArray(Database &db, QJsonArray &ja)
{
    while (db.next()) {
        QJsonObject jo;
        for (int i = 0; i < db.columnCount(); i++) {
            switch (db.value(i).type()) {
            case QVariant::Int:
            case QVariant::Char:
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

bool JsonReponse::checkAPIandFCMToken()
{
    fDb[":f_apikey"] = fJsonIn["pkServerAPIKey"];
    fDb.exec("select * from mgapp_devices where f_apikey=:f_apikey");
    if (!fDb.next()) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = lkInvalidAPIKey;
        return false;
    }
    if (fJsonIn["pkFcmToken"].toString() != fDb.string("f_fcmtoken")) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = lkDeviceNotRegistered;
        return false;
    }
    fDeviceId = fDb.integer("f_id");
    return true;
}

void JsonReponse::registerDevice()
{
    fDb[":f_apikey"] = fJsonIn["pkServerAPIKey"].toString();
    fDb.exec("select * from mgapp_devices where f_apikey=:f_apikey");
    if (!fDb.next()) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = lkInvalidAPIKey;
        return;
    }
    int id = fDb.integer("f_id");
    fDb[":f_fcmtoken"] = fJsonIn["pkFcmToken"];
    fDb.update("mgapp_devices", "f_id",  id);
    fJsonOut["ok"] = 1;
    fJsonOut["pkServerAPIKey"] = fJsonIn["pkServerAPIKey"];
}

void JsonReponse::login()
{
    fDb[":f_altpassword"] = fJsonIn["pin"];
    fDb.exec("select * from s_user where f_altpassword=md5(:f_altpassword)");
    if (!fDb.next()) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = lkInvalidPin;
        return;
    }
    fJsonOut["ok"] = 1;
    fJsonOut["pkPassHash"] = fDb.uuid();
    fJsonOut["pkFirstName"] = fDb.string("f_first");
    fJsonOut["pkLastName"] = fDb.string("f_last");
    fDb[":f_passhash"] = fJsonOut["pkPassHash"].toString();
    fDb.update("s_user", "f_id", fDb.integer("f_id"));
}

void JsonReponse::downloadData()
{
    QJsonArray ja;
    fDb[":f_enabled"] = 1;
    if (!fDb.exec("select g.f_id as `id`, gr.f_name as `groupName`, g.f_name as `goodsName`, "
            "gp.f_price1 as `price1`, gp.f_price2 as `price2`  "
            "from c_goods g "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_goods_prices gp on gp.f_goods=g.f_id "
            "where g.f_enabled=:f_enabled ")) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = fDb.lastDbError();
        return;
    }
    dbToArray(fDb, ja);
    fJsonOut["goods"] = ja;

    if (!fDb.exec("select pc.f_name as category, pg.f_name as `group`, ps.f_name as status, p.f_id as id, p.f_name as name, "
            "p.f_address as address, p.f_taxname as taxName, p.f_taxcode as taxCode, p.f_contact as contact, "
            "p.f_phone as phonenumber, p.f_permanent_discount as discount, p.f_price_politic as pricepolitic "
            "from c_partners p "
            "left join c_partners_state ps on ps.f_id=p.f_state "
            "left join c_partners_group pg on pg.f_id=p.f_group "
            "left join c_partners_category pc on pc.f_id=p.f_category ")) {
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = fDb.lastDbError();
        return;
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["partners"] = ja;

    fJsonOut["ok"] = 1;
}

void JsonReponse::checkHashOfPass()
{
    fDb[":f_passhash"] = fJsonIn["pkPassHash"];
    if (!fDb.exec("select * from s_user where f_passhash=:f_passhash")) {
        fJsonOut[pkOk] = 0;
        fJsonOut[pkMessage] = fDb.lastDbError();
        return;
    }
    fJsonOut[pkOk] = fDb.next() ? 1 : 0;
}

void JsonReponse::stock()
{
    int stock = fJsonIn[pkStock].toInt();
    QString sql = "select g.f_name as goodsname, sum(s.f_qty*s.f_sign) as qty "
            "from a_store s "
            "left join c_goods g on g.f_id=s.f_goods ";
    if (stock > 0) {
        sql += QString(" where s.f_store=%1 ").arg(stock);
    }
    sql += " group by g.f_name";
    if (!fDb.exec(sql)) {
        fJsonOut[pkOk] = 0;
        fJsonOut[pkMessage] = fDb.lastDbError();
        return;
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkStock] = ja;
    fJsonOut[pkOk] = 1;
}

void JsonReponse::saveOrder()
{

}
