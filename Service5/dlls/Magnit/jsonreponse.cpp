#include "jsonreponse.h"
#include "actions.h"
#include "locale.h"
#include "armsoft.h"

JsonReponse::JsonReponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void JsonReponse::getResponse()
{
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
    case hqSaveOrder:
        saveOrder();
        break;
    case hqPreorders:
        preordersList();
        break;
    case hqPreorderDetails:
        preorderDetails();
        break;
    case hqDebt:
        getDebts();
        break;
    case hqRoute:
        getRoute();
        break;
    case hqPreorderStock:
        orderList();
        break;
    case hqOpenOrder:
        openOrder();
        break;
    case hqRemoveOrderRow:
        removeOrderRow();
        break;
    case hqExportToAS:
        exportToAS();
        break;
    case hqDriverList:
        getDriversList();
        break;
    case hqVisit:
        visit();
        break;
    case hqCompleteDelivery:
        completeDelivery();
        break;
    default:
        fJsonOut["ok"] = 0;
        fJsonOut["message"] = QString("%1 %2").arg(lkUnknownProtocol, fJsonIn["pkAction"].toString());
        break;
    }
}

bool JsonReponse::dbFail()
{
    fJsonOut = QJsonObject();
    fJsonOut[pkOk] = 0;
    fJsonOut[pkMessage] = fDb.lastDbError();
    return false;
}

bool JsonReponse::error(const QString &err)
{
    fJsonOut = QJsonObject();
    fJsonOut[pkOk] = 0;
    fJsonOut[pkMessage] = err;
    return false;
}

bool JsonReponse::ok()
{
    fJsonOut[pkOk] = 1;
    return true;
}

void JsonReponse::dbToArray(Database &db, QJsonArray &ja)
{
    while (db.next()) {
        QJsonObject jo;
        for (int i = 0; i < db.columnCount(); i++) {
            //qDebug() << db.value(i).type();
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
    //If pass hash not exists, use username and password to authenticate
    if (fJsonIn[pkAction].toInt() != hqLogin && fJsonIn[pkAction].toInt() != hqRegisterDevice && fJsonIn.contains(pkUsername)) {
        fDb[":f_login"] = fJsonIn[pkUsername].toString();
        fDb[":f_password"] = fJsonIn[pkPassword].toString();
        fDb.exec("select * from s_user where f_login=:f_login and f_password=md5(:f_password)");
        if (!fDb.next()) {
            fJsonOut["ok"] = 0;
            fJsonOut["message"] = lkInvalidCredentials;
            return false;
        }
        fUserId = fDb.integer("f_id");
        return true;
    }
    //user password hash
    if (fJsonIn[pkAction].toInt() != hqLogin && fJsonIn[pkAction].toInt() != hqRegisterDevice) {
        fDb[":f_passhash"] = fJsonIn[pkPassHash].toString();
        fDb.exec("select * from s_user where f_passhash=:f_passhash");
        if (!fDb.next()) {
            fJsonOut["ok"] = 0;
            fJsonOut["message"] = lkInvalidHash;
            return false;
        }
        fUserId = fDb.integer("f_id");
    }
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
    fJsonOut[pkDriver] = fDb.integer("f_id");
    fJsonOut["pkFirstName"] = fDb.string("f_first");
    fJsonOut["pkLastName"] = fDb.string("f_last");
    fDb[":f_passhash"] = fJsonOut["pkPassHash"].toString();
    fDb.update("s_user", "f_id", fDb.integer("f_id"));
}

bool JsonReponse::downloadData()
{
    QJsonArray ja;
    fDb[":f_enabled"] = 1;
    if (!fDb.exec("select g.f_id as `id`, gr.f_name as `groupName`, g.f_name as `goodsName`, "
                  "gp.f_price1 as `price1`, gp.f_price2 as `price2`, 0 as specialflag, "
                  "g.f_lowlevel as qtystop  "
                  "from c_goods g "
                  "left join c_groups gr on gr.f_id=g.f_group "
                  "left join c_goods_prices gp on gp.f_goods=g.f_id "
                  "where g.f_enabled=:f_enabled "
                  "order by g.f_queue, g.f_name ")) {
        return dbFail();
    }
    dbToArray(fDb, ja);
    fJsonOut["goods"] = ja;

    ja = QJsonArray();
    fDb[":f_enabled"] = 1;
    if (!fDb.exec("select distinct(gr.f_id) as `id`, gr.f_name as `name` "
                  "from c_goods g "
                  "left join c_groups gr on gr.f_id=g.f_group "
                  "where g.f_enabled=:f_enabled ")) {
        return dbFail();
    }
    dbToArray(fDb, ja);
    fJsonOut["goodsgroup"] = ja;

    if (!fDb.exec("select pc.f_name as category, pg.f_name as `group`, ps.f_name as status, p.f_id as id, p.f_name as name, "
                  "p.f_address as address, p.f_taxname as taxName, p.f_taxcode as taxCode, p.f_contact as contact, "
                  "p.f_phone as phonenumber, p.f_permanent_discount as discount, p.f_price_politic as pricepolitic "
                  "from c_partners p "
                  "left join c_partners_state ps on ps.f_id=p.f_state "
                  "left join c_partners_group pg on pg.f_id=p.f_group "
                  "left join c_partners_category pc on pc.f_id=p.f_category ")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["partners"] = ja;

    if (!fDb.exec("select f_partner as partner, f_goods as goods, f_price as price from c_goods_special_prices")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["specialprices"] = ja;

    if (!fDb.exec("select f_id as id, f_name as name from c_storages where f_state=1 ")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["storages"] = ja;

    if (!fDb.exec("select distinct(u.f_id) as id, concat_ws(' ', u.f_last, u.f_first) as name "
                  "from o_route_drivers r "
                  "left join s_user u on u.f_id=r.f_user ")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["drivers"] = ja;

    fDb[":f_id"] = fUserId;
    if (!fDb.exec("select f_config from s_user where f_id=:f_id")) {
        return dbFail();
    }
    QJsonObject jConfig;
    fDb.next();
    int config = fDb.integer("f_config");
    if (config == 0) {
        jConfig["storage"] = 1;
    } else {
        fDb[":f_settings"] = config;
        fDb.exec("select f_key, f_value from s_settings_values "
                "where f_settings=:f_settings and f_key in (14) ");
        while (fDb.next()) {
            switch (fDb.integer("f_key")) {
            case 14: jConfig["storage"] = fDb.string("f_value").toInt();
            }
        }
    }
    fJsonOut["config"] = jConfig;

    fJsonOut["ok"] = 1;
    return true;
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

bool JsonReponse::stock()
{
    int stock = fJsonIn[pkStock].toInt();
    int group = fJsonIn[pkGroup].toInt();
    QString sql = "select s.f_goods as goodsid, gr.f_name as groupname, g.f_name as goodsname, sum(s.f_qty*s.f_type) as qty "
            "from a_store s "
            "left join c_goods g on g.f_id=s.f_goods "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_storages st on st.f_id=s.f_store ";
    QString where = "where g.f_enabled=1 and st.f_state=1 ";
    if (stock > 0) {
        where += QString(" and s.f_store=%1 ").arg(stock);
    }
    if (group > 0) {
        where += QString(" and g.f_group=%1").arg(group);
    }
    sql += where;
    sql += " group by g.f_name";
    if (!fDb.exec(sql)) {
        return dbFail();
    }
    QJsonArray ja;
    dbToArray(fDb, ja);

    fJsonOut[pkData] = ja;
    fJsonOut[pkOk] = 1;
    return true;
}

bool JsonReponse::saveOrder()
{
    QMap<QString, QVariant> sale;
    sale[":f_id"] = fJsonIn["orderid"].toString();
    sale[":f_staff"] = fJsonIn["executor"].toInt();
    sale[":f_cashier"] = fUserId;
    sale[":f_partner"] = fJsonIn["partner"]["id"].toInt();
    sale[":f_date"] = QDate::currentDate();
    sale[":f_time"] = QTime::currentTime();
    sale[":f_state"] = 1;
    sale[":f_payment"] = fJsonIn["paymenttype"].toInt();
    sale[":f_saletype"] = fJsonIn["partner"]["pricepolitic"].toInt();
    sale[":f_discount"] = fJsonIn["partner"]["discount"].toDouble() / 100;
    sale[":f_comment"] = fJsonIn["comment"].toString();
    sale[":f_datefor"] = QDate::fromString(fJsonIn["deliverydate"].toString(), "dd/MM/yyyy");

    QList<QMap<QString, QVariant> > saleBody;

    QMap<QString, QVariant> back;
    back[":f_staff"] = fUserId;
    back[":f_cashier"] = fUserId;
    back[":f_partner"] = fJsonIn["partner"]["id"].toInt();
    back[":f_date"] = QDate::currentDate();
    back[":f_time"] = QTime::currentTime();
    back[":f_state"] = 5;
    sale[":f_payment"] = fJsonIn["paymenttype"].toInt();
    back[":f_saletype"] = 3;
    back[":f_discount"] = fJsonIn["partner"]["discount"].toDouble() / 100;
    back[":f_comment"] = fJsonIn["comment"].toString();

    QList<QMap<QString, QVariant> > backBody;

    QJsonArray ja = fJsonIn["goods"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject o = ja.at(i).toObject();
        if (o["qtysale"].toDouble() > 0.001) {
            QMap<QString, QVariant> sq;
            sq[":f_id"] = o["dbuuid"].toString();
            sq[":f_goods"] = o["id"].toInt();
            sq[":f_qty"] = o["qtysale"].toDouble();
            sq[":f_state"] = 1;
            sq[":f_store"] = fJsonIn["storage"].toInt();
            sq[":f_price"] = o["price"].toDouble();
            sq[":f_discount"] = o["discount"].toDouble();
            sq[":f_special_price"] = o["specialflag"].toInt();
            saleBody.append(sq);
            sale[":f_amount"] = sale[":f_amount"].toDouble() + (o["qtysale"].toDouble() * o["price"].toDouble());
        }
        if (o["qtyback"].toDouble() > 0.001) {
            QMap<QString, QVariant> sq;
            sq[":f_goods"] = o["id"].toInt();
            sq[":f_qty"] = o["qtyback"].toDouble();
            sq[":f_state"] = 1;
            sq[":f_store"] = fJsonIn["storage"].toInt();;
            sq[":f_price"] = o["price"].toDouble();
            backBody.append(sq);
            back[":f_amount"] = back[":f_amount"].toDouble() + (o["qtyback"].toDouble() * o["price"].toDouble());
        }
    }

    fDb.startTransaction();
    if (!saleBody.isEmpty()) {
        bool u = !sale[":f_id"].toString().isEmpty();
        if (!u) {
            sale[":f_id"] = fDb.uuid();
        }
        for (QMap<QString, QVariant>::const_iterator it = sale.constBegin(); it != sale.constEnd(); it++) {
            fDb[it.key()] = it.value();
        }
        if (u) {
            if (!fDb.update("o_draft_sale", "f_id", sale[":f_id"])) {
                return dbFail();
            }
        } else {
            if (!fDb.insert("o_draft_sale")) {
                fDb.rollback();
                return dbFail();
            }
            fDb[":f_partner"] = fJsonIn["partner"]["id"].toInt();
            fDb[":f_action"] = 2;
            fDb[":f_date"] = QDate::currentDate();
            fDb[":f_driver"] = fUserId;
            fDb[":f_action_datetime"] = QDateTime::currentDateTime();
            fDb[":f_action_comment"] = QString("Վաճառք");
            fDb[":f_saleuuid"] = sale[":f_id"];
            if (!fDb.insert("o_route_exec")) {
                return dbFail();
            }

        }
        for (QMap<QString, QVariant> &l: saleBody) {
            u = !l[":f_id"].toString().isEmpty();
            if (!u) {
                l[":f_id"] = fDb.uuid();
            }
            l[":f_header"] = sale[":f_id"];
            for (QMap<QString, QVariant>::const_iterator lt = l.constBegin(); lt != l.constEnd(); lt++) {
                fDb[lt.key()] = lt.value();
            }
            if (u) {
                if (!fDb.update("o_draft_sale_body", "f_id", l[":f_id"])) {
                    return dbFail();
                }
            } else {
                if (!fDb.insert("o_draft_sale_body")) {
                    fDb.rollback();
                    return dbFail();
                }
            }
        }
    }

    if (!backBody.isEmpty()) {
        back[":f_id"] = fDb.uuid();
        for (QMap<QString, QVariant>::const_iterator it = back.constBegin(); it != back.constEnd(); it++) {
            fDb[it.key()] = it.value();
        }

        if (!fDb.insert("o_draft_sale")) {
            fDb.rollback();
            return dbFail();
        }
        for (QMap<QString, QVariant> &l: backBody) {
            l[":f_header"] = back[":f_id"];
            l[":f_id"] = fDb.uuid();
            for (QMap<QString, QVariant>::const_iterator lt = l.constBegin(); lt != l.constEnd(); lt++) {
                fDb[lt.key()] = lt.value();
            }
            if (!fDb.insert("o_draft_sale_body")) {
                fDb.rollback();
                return dbFail();
            }
        }
    }

    fDb.commit();
    fJsonOut[pkOk] = 1;
    return true;
}

void JsonReponse::preordersList()
{
    int state = fJsonIn["state"].toInt();
    qDebug() << fJsonIn[pkDate].toString();
    QDate date = QDate::fromString(fJsonIn[pkDate].toString(), "dd/MM/yyyy");
    int driver = fJsonIn[pkDriver].toInt();
    if (!date.isValid()) {
        date = QDate::currentDate();
    }
    QString where = "where p.f_id>0 ";
    if (state > 0) {
        where += QString(" and o.f_state=%1").arg(state);
    }
    if (driver > 0) {
        where += QString(" and o.f_staff=%1").arg(driver);
    }
    switch (state) {
    case 1:
        where += " and o.f_date=:f_datefor";
        break;
    case 2:
        where += " and o.f_datefor=:f_datefor";
        break;
    default:
        where += " and o.f_datefor=:f_datefor";
        break;
    }

    fDb[":f_datefor"] = date;
    if (!fDb.exec(QString("select o.f_id as `id`, date_format(o.f_date, '%d/%m/%Y')as `date`, "
                          "p.f_name as `partnername`, p.f_address as `address`, o.f_amount as `amount`, "
                          "o.f_payment as payment, o.f_state as state "
                          "from o_draft_sale o "
                          "left join c_partners p on p.f_id=o.f_partner %1 "
                          "order by o.f_date ").arg(where))) {
        fJsonOut[pkOk] = 0;
        fJsonOut[pkMessage] = fDb.lastDbError();
        fDb.rollback();
        return;
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkData] = ja;
    fJsonOut[pkOk] = 1;
}

void JsonReponse::preorderDetails()
{
    QString id = fJsonIn["id"].toString();
    fDb[":f_header"] = id;
    if (!fDb.exec("select od.f_state as state, gr.f_name as groupname, g.f_name as goodsname, ob.f_qty as qty, "
                  "ob.f_price as price "
                  "from o_draft_sale_body ob "
                  "left join o_draft_sale od on od.f_id=ob.f_header "
                  "left join c_goods g on g.f_id=ob.f_goods "
                  "left join c_groups gr on gr.f_id=g.f_group "
                  "where ob.f_header=:f_header and ob.f_state=1 ")) {
        fJsonOut[pkOk] = 0;
        fJsonOut[pkMessage] = fDb.lastDbError();
        fDb.rollback();
        return;
    }

    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkData] = ja;
    fJsonOut[pkOk] = 1;
}

void JsonReponse::getDebts()
{
    int partner = fJsonIn["partner"].toInt();
    QString where = "where p.f_id>0 ";
    if (partner > 0) {
        where += QString(" and bc.f_costumer=%1").arg(partner);
    }
    if (!fDb.exec(QString("select p.f_id as partnerid, p.f_name as name, p.f_address as address, sum(bc.f_amount) as amount "
                          "from b_clients_debts bc "
                          "left join c_partners p on p.f_id=bc.f_costumer %1 "
                          "group by p.f_id ").arg(where))) {
        fJsonOut[pkOk] = 0;
        fJsonOut[pkMessage] = fDb.lastDbError();
        fDb.rollback();
        return;
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkData] = ja;
    fJsonOut[pkOk] = 1;
}

bool JsonReponse::orderList()
{
    int stock = fJsonIn[pkStock].toInt();
    int group = fJsonIn[pkGroup].toInt();
    QString date = fJsonIn[pkDate].toString();
    QString sql = "select o.f_datefor, st.f_name as stockname, g.f_id as goodsid, gr.f_name as groupname, g.f_name as goodsname, sum(s.f_qty) as qty "
           "from o_draft_sale_body s "
           "left join o_draft_sale o on o.f_id=s.f_header "
           "left join c_storages st on st.f_id=s.f_store "
           "left join c_goods g on g.f_id=s.f_goods "
           "left join c_groups gr on gr.f_id=g.f_group ";
    QString where = "where g.f_enabled=1 and o.f_state<4 and o.f_saletype in (1,2) ";
    if (stock > 0) {
        where += QString(" and s.f_store=%1 ").arg(stock);
    }
    if (group > 0) {
        where += QString(" and g.f_group=%1").arg(group);
    }
    sql += where;
    sql += " group by g.f_name";
    if (!fDb.exec(sql)) {
        return dbFail();
    }

    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkOk] = 1;
    fJsonOut[pkData] = ja;
    return true;
}

bool JsonReponse::getRoute()
{
    QDate date = QDate::fromString(fJsonIn[pkDate].toString(), "dd/MM/yyyy");
    if (!date.isValid()) {
        date = QDate::currentDate();
    }
    int driver = fJsonIn[pkDriver].toInt();
    if (driver == 0) {
        driver = fUserId;
    }
    int day = date.dayOfWeek();
    fDb.exec("select f_started, f_rounds from o_route_round");
    fDb.next();
    QDate started = fDb.date("f_started");
    int rounds = fDb.integer("f_rounds");
    int daysAfter = started.daysTo(date);
    int dayOfRound = (daysAfter % (7 * rounds));
    int round = dayOfRound / 7;
    round++;
    fDb[":f_driver"] = driver;
    fDb[":f_round"] = round;
    fDb[":f_datefor"] = date;
    if (!fDb.exec(QString("select r.f_partner as partnerid, p.f_name as partnername, p.f_address as address, cast(count(o.f_id) as signed) as orders, "
                          "coalesce(re.f_action, 0) as `action` "
                          "from o_route r  "
                          "left join o_draft_sale o on o.f_partner=r.f_partner and o.f_datefor=:f_datefor "
                          "left join o_route_exec re on re.f_partner=r.f_partner and re.f_date=:f_datefor "
                          "left join c_partners p on p.f_id=r.f_partner "
                          "where (r.f_%1=1 and r.f_driver=:f_driver and r.f_round=:f_round) or (weekday(o.f_datefor)=%2 and o.f_state=2)  "
                          "group by 1, 5 "
                          "order by f_q").arg(day).arg(day - 1))) {
        return dbFail();
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkOk] = 1;
    fJsonOut[pkData] = ja;
    return true;
}

bool JsonReponse::openOrder()
{
    QString orderId = fJsonIn["id"].toString();
    fDb[":f_id"] = orderId;
    if (!fDb.exec("select o.f_partner, o.f_comment as comment, o.f_payment as paymenttype "
                  "from o_draft_sale o where f_id=:f_id")) {
        return dbFail();
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    if (ja.count() == 0) {
        return error(lkInvalidOrderId);
    }
    QJsonObject jorder = ja.at(0).toObject();
    fJsonOut["order"] = jorder;
    fDb[":f_header"] = orderId;
    fDb[":f_state"] = 1;
    if (!fDb.exec("select dg.f_id as dbuuid, dg.f_id as intuuid, dg.f_goods as id, gr.f_name as groupname, g.f_name as goodsname, "
                  "dg.f_price as price, dg.f_qty as qtySale, 0 as qtyBack, dg.f_store as storage, 0 as price1, 0 as price2 "
                  "from o_draft_sale_body dg "
                  "left join c_goods g on g.f_id=dg.f_goods "
                  "left join c_groups gr on gr.f_id=g.f_group "
                  "where dg.f_header=:f_header and dg.f_state=:f_state")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    fJsonOut["goods"] = ja;

    fDb[":f_id"] = jorder["f_partner"].toInt();
    if (!fDb.exec("select pc.f_name as category, pg.f_name as `group`, ps.f_name as status, p.f_id as id, p.f_name as name, "
                  "p.f_address as address, p.f_taxname as taxName, p.f_taxcode as taxCode, p.f_contact as contact, "
                  "p.f_phone as phonenumber, p.f_permanent_discount as discount, p.f_price_politic as pricepolitic "
                  "from c_partners p "
                  "left join c_partners_state ps on ps.f_id=p.f_state "
                  "left join c_partners_group pg on pg.f_id=p.f_group "
                  "left join c_partners_category pc on pc.f_id=p.f_category "
                  "where p.f_id=:f_id")) {
        return dbFail();
    }
    ja = QJsonArray();
    dbToArray(fDb, ja);
    if (ja.count() == 0) {

    } else {
        fJsonOut["partner"] = ja.at(0).toObject();
    }
    fJsonOut[pkOk] = 1;
}

bool JsonReponse::removeOrderRow()
{
    fDb[":f_state"] = 2;
    fDb[":f_userremove"] = fUserId;
    fDb[":f_dateremoved"] = QDate::currentDate();
    fDb[":f_timeremoved"] = QTime::currentTime();
    if (!fDb.update("o_draft_sale_body", "f_id", fJsonIn["dbuuid"].toString())) {
        return dbFail();
    }
    return ok();
}

bool JsonReponse::exportToAS()
{
    QString err;
    ArmSoft a(fJsonIn);
    if (!a.exportToAS(fJsonIn["draftid"].toString(), err)) {
        return error(err);
    }
    fJsonOut[pkOk] = 1;
    return true;
}

bool JsonReponse::getDriversList()
{
    if (!fDb.exec("select distinct(u.f_id) as id, concat_ws(' ', u.f_last, u.f_first) as name "
                  "from o_route r "
                  "left join s_user u on u.f_id=r.f_driver ")) {
        return dbFail();
    }
    QJsonArray ja;
    dbToArray(fDb, ja);
    fJsonOut[pkOk] = 1;
    fJsonOut[pkData] = ja;
    return true;
}

bool JsonReponse::visit()
{
    fDb[":f_partner"] = fJsonIn["partner"].toInt();
    fDb[":f_action"] = fJsonIn["action"].toInt();
    fDb[":f_date"] = QDate::currentDate();
    fDb[":f_driver"] = fUserId;
    fDb[":f_action_datetime"] = QDateTime::currentDateTime();
    switch (fJsonIn["action"].toInt()) {
    case 1:
        fDb[":f_action_comment"] = QString("Այց, ապրանք պետք չէ");
        break;
    case 2:
        fDb[":f_action_comment"] = QString("Վաճառք");
        break;
    case 3:
        fDb[":f_action_comment"] = QString("Խանութը փակ էր");
        break;
    }
    if (!fDb.insert("o_route_exec")) {
        return dbFail();
    }
    fJsonOut[pkOk] = 1;
    return true;
}

bool JsonReponse::completeDelivery()
{
    fDb[":f_state"] = 3;
    if (!fDb.update("o_draft_sale", "f_id", fJsonIn["id"].toString())) {
        return dbFail();
    }
    fDb[":f_id"] = fJsonIn["id"].toString();
    if (!fDb.exec("select * from o_draft_sale where f_id=:f_id")) {
            return dbFail();
    }
    if (!fDb.next()) {
        return error("Invalid order id");
    }

    fDb[":f_partner"] = fDb.integer("f_partner");
    fDb[":f_action"] = 2;
    fDb[":f_date"] = QDate::currentDate();
    fDb[":f_driver"] = fUserId;
    fDb[":f_action_datetime"] = QDateTime::currentDateTime();
    fDb[":f_action_comment"] = QString("Պատվերը առաքված է");
    fDb[":f_saleuuid"] = fJsonIn["id"].toString();
    if (!fDb.insert("o_route_exec")) {
        return dbFail();
    }

    return ok();
}
