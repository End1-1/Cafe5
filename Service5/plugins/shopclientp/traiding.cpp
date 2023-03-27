#include "traiding.h"
#include "datadriver.h"
#include "ops.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void create_empty_draft(RawMessage &rm, Database &db, const QByteArray &in, int user)
{
    QString newid = db.uuid();
    db[":f_id"] = newid;
    db[":f_state"] = 1;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_staff"] = user;
    db[":f_amount"] = 0;
    if (db.insert("o_draft_sale") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    db[":f_id"] = newid;
    if (db.exec("select s.f_id as `uuid`, s.f_date as `date`, s.f_partner as `partner`, s.f_debt as `isdebt`, "
                "s.f_amount as `amount`, s.f_discount as `discount` "
                "from o_draft_sale s where s.f_id=:f_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Invalid document header uuid");
        return;
    }
    QJsonObject jo;
    fetchDbToJsonObject(jo, db);
    rm.putUByte(1);
    rm.putString(newid);
    rm.putString(QJsonDocument(jo).toJson(QJsonDocument::Compact));
}

void get_drafts_headers(RawMessage &rm, Database &db, const QByteArray &in, int user)
{
    db[":f_staff"] = user;
    data_list2(rm, db, "select dr.f_id, dr.f_date as `Ամսաթիվ`, dr.f_time as `Ժամ`, p.f_taxname as `Գործընկեր`, dr.f_amount as `Գումար` "
                "from o_draft_sale dr "
                "left join c_partners p on p.f_id=dr.f_partner "
                "where dr.f_staff=:f_staff and dr.f_state=1");
}

void add_goods_to_draft(RawMessage &rm, Database &db, const QByteArray &in, int user)
{
    QString newid, header;
    quint32 goods, state;
    double qty, back, price;
    rm.readString(newid, in);
    rm.readString(header, in);
    rm.readUInt(goods, in);
    rm.readUInt(state, in);
    rm.readDouble(qty, in);
    rm.readDouble(back, in);
    rm.readDouble(price, in);

    db[":f_header"] = header;
    db[":f_state"] = state;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_back"] = back;
    db[":f_price"] = price;
    if (newid.isEmpty()) {
        newid = db.uuid();
        db[":f_id"] = newid;
        db[":f_dateappend"] = QDate::currentDate();
        db[":f_timeappend"] = QTime::currentTime();
        db[":f_userappend"] = user;
        if (db.insert("o_draft_sale_body") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
    } else {
        if (state == 2) {
            db[":f_dateremoved"] = QDate::currentDate();
            db[":f_timeremoved"] = QTime::currentTime();
            db[":f_userremove"] = user;
        }
        if (db.update("o_draft_sale_body", "f_id", newid) == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
    }
    db[":f_id"] = header;
    db.exec("update o_draft_sale set f_amount=(select sum(f_qty*f_price) from o_draft_sale_body where f_state=1 and f_header=:f_id) where f_id=:f_id");
    db[":f_id"] = header;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    db.next();
    double totalamount = db.doubleValue("f_amount");

    db[":f_id"] = goods;
    db.exec("select f_name from c_goods where f_id=:f_id");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    QString name = db.string("f_name");

    rm.putUByte(1);
    rm.putString(newid);
    rm.putUInt(state);
    rm.putUInt(goods);
    rm.putString(name);
    rm.putDouble(qty);
    rm.putDouble(back);
    rm.putDouble(price);
    rm.putDouble(totalamount);
}

bool open_draft_header(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString id;
    rm.readString(id, in);
    db[":f_saleuuid"] = id;
    if (!db.exec("select f_action from o_route_exec where f_saleuuid=:f_saleuuid")) {
        return rmerror(rm, db.lastDbError());
    }
    if (db.next()) {
        if (db.integer("f_action") == 1) {
            return rmerror(rm, "Blocked");
        }
    }

    db[":f_id"] = id;
    if (db.exec("select s.f_id as `uuid`, s.f_date as `date`, s.f_partner as `partner`, s.f_debt as `isdebt`, "
                "s.f_amount as `amount`, s.f_discount as `discount` "
                "from o_draft_sale s where s.f_id=:f_id") == false) {
        return rmerror(rm, db.lastDbError());
    }
    if (db.next() == false) {
        return rmerror(rm, "Invalid document header uuid");
    }
    QJsonObject jo;
    fetchDbToJsonObject(jo, db);
    rm.putUByte(1);
    rm.putString(QJsonDocument(jo).toJson(QJsonDocument::Compact));
    return true;
}

void open_draft_body(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString header;
    rm.readString(header, in);
    db[":f_header"] = header;
    data_list2(rm, db, "select db.f_id, db.f_goods, db.f_qty, db.f_back, db.f_price, g.f_name "
              "from o_draft_sale_body db "
              "left join c_goods g on g.f_id=db.f_goods "
              "where db.f_state=1 and db.f_header=:f_header ");
}

void fetchDbToJsonObject(QJsonObject &jo, Database &db) {
    for (int i = 0; i < db.columnCount(); i++) {
        qDebug() << "Value type" << db.value(i).type();
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
    qDebug() << "JSON OBJECT" << jo;
}

void fetchDbToJsonArray(QJsonArray &ja, Database &db) {
    while (db.next()) {
        QJsonObject jo;
        fetchDbToJsonObject(jo, db);
        ja.append(jo);
    }
    qDebug() << ja;
}

bool getJsonOfQuery(RawMessage &rm, Database &db, const QByteArray &in, int jsonOf, int userId)
{
    QJsonArray ja;
    switch (jsonOf) {
    case op_json_partners_list:
        if (!db.exec("select p.f_id as id, f_taxname as taxname, f_taxcode as taxcode, "
                "f_contact as contact, f_phone as phone, d.f_value as `discount`, p.f_address as `address` "
                "from c_partners p "
                "left join b_cards_discount d on d.f_client=p.f_id "
                "order by f_taxname ")) {
            return rmerror(rm, db.lastDbError());
        }

        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    case op_json_predefined_goods:
        if (!db.exec("select pl.f_goods as id, g.f_name as name, g.f_scancode as barcode, "
                     "gpr.f_price1 as price1, gpr.f_price2 as price2 "
                     "from c_goods_predefined_list pl "
                     "left join c_goods g on g.f_id=pl.f_goods "
                     "left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 ")) {
            return rmerror(rm, db.lastDbError());
        }
        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    case op_json_debts:
        if (!db.exec("select p.f_id as partner, p.f_taxname as taxname, o.f_datecash, a.f_amount*a.f_dc as amount "
                     "from a_dc a "
                     "left join o_header o on o.f_id=a.f_id "
                     "left join c_partners p on p.f_id=o.f_partner ")) {
            return rmerror(rm, db.lastDbError());
        }
        rm.putUByte(1);
        return true;
    case op_json_storagenames:
        if (!db.exec("select f_id as id, f_name as name from c_storages")) {
            return rmerror(rm, db.lastDbError());
        }
        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    case op_json_stock:
        quint32 storage;
        rm.readUInt(storage, in);
        db[":f_state"] = 1;
        db[":f_store"] = storage;
        if (!db.exec("select gr.f_name as groupname, g.f_name as name, sum(s.f_qty*s.f_type) as qty "
                     "from a_store s "
                     "inner join a_header h on h.f_id=s.f_document "
                     "inner join c_goods g on g.f_id=s.f_goods "
                     "inner join c_groups gr on gr.f_id=g.f_group "
                     "where h.f_state=:f_state and s.f_store=:f_store "
                     "group by 1, 2")) {
            return rmerror(rm, db.lastDbError());
        }
        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    case op_json_sales_history: {
        QString date1Str, date2Str;
        rm.readString(date1Str, in);
        rm.readString(date2Str, in);
        QDate d1 = QDate::fromString(date1Str, "dd/MM/yyyy");
        QDate d2 = QDate::fromString(date2Str, "dd/MM/yyyy");
        db[":f_state"] = 2;
        db[":f_datecash1"] = d1;
        db[":f_datecash2"] = d2;
        if (!db.exec("select o.f_id as `id`, o.f_datecash as `date`, p.f_taxcode as `taxcode`, p.f_taxname as `taxname`, "
                     "o.f_amounttotal as `amount` "
                     "from o_header o "
                     "left join c_partners p on p.f_id=o.f_partner "
                     "where o.f_state=:f_state and o.f_datecash between :f_datecash1 and :f_datecash2 ")) {
            return rmerror(rm, db.lastDbError());
        }
        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    }
    case op_json_route:
        db[":f_date"] = QDate::currentDate();
        if (!db.exec(("select r.f_id as `id`, r.f_partner as `partner`, p.f_address as `address`, p.f_taxname as `taxname`, p.f_taxcode as `taxcode`,"
                      "r.f_action as `action` "
                      "from o_route_exec r "
                      "inner join c_partners p on p.f_id=r.f_partner "
                      "where r.f_date=:f_date"))) {
            return rmerror(rm, db.lastDbError());
        }
        fetchDbToJsonArray(ja, db);
        rm.putUByte(1);
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    case op_json_route_update: {
        quint32 routeId, partnerId, action;
        QString comment, saleUuid;
        rm.readUInt(routeId, in);
        rm.readUInt(partnerId, in);
        rm.readUInt(action, in);
        rm.readString(comment, in);
        rm.readString(saleUuid, in);
        if (routeId == 0) {
            db[":f_partner"] = partnerId;
            db[":f_date"] = QDate::currentDate();
            db[":f_driver"] = userId;
            int newrouteid;
            if (!db.insert("o_route_exec", newrouteid)) {
                return rmerror(rm, db.lastDbError());
            }
            routeId = newrouteid;
        }
        db[":f_id"] = routeId;
        db[":f_action"] = action;
        db[":f_action_comment"] = comment;
        db[":f_action_datetime"] = QDateTime::currentDateTime();
        db[":f_saleuuid"] = saleUuid;
        if (!db.update("o_route_exec", "f_id", routeId)) {
            return rmerror(rm, db.lastDbError());
        }
        rm.putUByte(1);
        rm.putString(QString("{\"result\":0,\"partner\":%1}").arg(partnerId));
        return true;
    }
    case op_json_stock_all: {
        rm.readUInt(storage, in);
        db[":f_store"] = storage;
        if (!db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as qty "
                     "from a_store s "
                     "where s.f_store=:f_store "
                     "group by 1")) {
            return rmerror(rm, db.lastDbError());
        }
        QMap<int, double> myStock;
        while (db.next()) {
            myStock[db.integer("f_goods")] = db.doubleValue("qty");
        }

        db[":f_store"] = storage;
        if (!db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as qty "
                     "from a_store s "
                     "where s.f_store<>:f_store "
                     "group by 1")) {
            return rmerror(rm, db.lastDbError());
        }
        QMap<int, double> otherStock;
        while (db.next()) {
            otherStock[db.integer("f_goods")] = db.doubleValue("qty");
        }

        if (!db.exec("select s.f_goods, sum(s.f_qty) as qty "
                     "from o_draft_sale_body s "
                     "where f_state=1 "
                     "group by 1")) {
            return rmerror(rm, db.lastDbError());
        }
        QMap<int, double> orderStock;
        while (db.next()) {
            orderStock[db.integer("f_goods")] = db.doubleValue("qty");
        }

        QMap<int, QJsonObject> totalStock;
        for (QMap<int, double>::const_iterator it = myStock.constBegin(); it != myStock.constEnd(); it++) {
            totalStock[it.key()]["myStock"] = it.value();
            totalStock[it.key()]["goods"] = it.key();
        }
        for (QMap<int, double>::const_iterator it = otherStock.constBegin(); it != otherStock.constEnd(); it++) {
            totalStock[it.key()]["otherStock"] = it.value();
            totalStock[it.key()]["goods"] = it.key();
        }
        for (QMap<int, double>::const_iterator it = orderStock.constBegin(); it != orderStock.constEnd(); it++) {
            totalStock[it.key()]["orderStock"] = it.value();
            totalStock[it.key()]["goods"] = it.key();
        }
        for (QMap<int, QJsonObject>::iterator it = totalStock.begin(); it != totalStock.end(); it++) {
            if (it.value()["myStock"].toDouble() < 0.001) {
                it.value()["myStock"] = 0;
            }
            if (it.value()["otherStock"].toDouble() < 0.001) {
                it.value()["otherStock"] = 0;
            }
            if (it.value()["orderStock"].toDouble() < 0.001) {
                it.value()["orderStock"] = 0;
            }
        }
        for (QMap<int, QJsonObject>::const_iterator it = totalStock.constBegin(); it != totalStock.constEnd(); it++) {
            ja.append(it.value());
        }

        rm.putUByte(1);
        qDebug() << ja;
        rm.putString(QJsonDocument(ja).toJson(QJsonDocument::Compact));
        return true;
    }
    case op_json_partner_debt: {
        quint32 partner;
        rm.readUInt(partner, in);
        rm.putUByte(1);
        rm.putString(QString("[{\"debt\":%1}]").arg(partner * 1000));
        return true;
    }
    }
    return true;
}

bool update_draft_header(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString jsonStr;
    rm.readString(jsonStr, in);
    QJsonParseError jsonErr;
    QJsonObject jo = QJsonDocument::fromJson(jsonStr.toUtf8(), &jsonErr).object();
    if (jsonErr.error != QJsonParseError::NoError) {
        return rmerror(rm, QString("JsonParseError in update_draft_header %1").arg(jsonErr.error));
    }
    db[":f_partner"] = jo["partner"].toInt();
    db[":f_debt"] = jo["isdebt"].toInt();
    if (!db.update("o_draft_sale", "f_id", jo["uuid"].toString())) {
        return rmerror(rm, db.lastDbError());
    }
    rm.putUByte(1);
    return true;
}
