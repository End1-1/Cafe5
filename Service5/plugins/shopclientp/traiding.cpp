#include "traiding.h"
#include "datadriver.h"

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
    rm.putUByte(1);
    rm.putString(newid);
}

void get_drafts_headers(RawMessage &rm, Database &db, const QByteArray &in, int user)
{
    db[":f_staff"] = user;
    data_list(rm, db, "sq_data_sales_drafts_headers");
}

void add_goods_to_draft(RawMessage &rm, Database &db, const QByteArray &in, int user)
{
    QString newid, header;
    quint32 goods, state;
    double qty, price;
    rm.readString(newid, in);
    rm.readString(header, in);
    rm.readUInt(goods, in);
    rm.readUInt(state, in);
    rm.readDouble(qty, in);
    rm.readDouble(price, in);

    db[":f_header"] = header;
    db[":f_state"] = state;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
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
    rm.putDouble(price);
    rm.putDouble(totalamount);
}

void open_draft_header(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString id;
    double amount;
    rm.readString(id, in);
    db[":f_id"] = id;
    if (db.exec("select * from o_draft_sale where f_id=:f_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Invalid document header uuid");
        return;
    }
    amount = db.doubleValue("f_amount");
    rm.putUByte(1);
    rm.putDouble(amount);
}

void open_draft_body(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString header;
    rm.readString(header, in);
    db[":f_header"] = header;
    data_list2(rm, db, "select db.f_id, db.f_goods, db.f_qty, db.f_price, g.f_name "
              "from o_draft_sale_body db "
              "left join c_goods g on g.f_id=db.f_goods "
              "where db.f_state=1 and db.f_header=:f_header ");
}
