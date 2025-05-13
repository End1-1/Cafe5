#include "odraftsale.h"

bool ODraftSale::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getString("f_id");
    state = db.getInt("f_state");
    saleType = db.getInt("f_saletype");
    date = db.getDate("f_date");
    time = db.getTime("f_time");
    cashier = db.getInt("f_cashier");
    staff = db.getInt("f_staff");
    partner = db.getInt("f_partner");
    amount = db.getDouble("f_amount");
    comment = db.getString("f_comment");
    payment = db.getInt("f_payment");
    deliveryDate = db.getDate("f_datefor");
    hall = db.getInt("f_hall");
    return true;
}

void ODraftSale::bind(C5Database &db)
{
    Q_ASSERT(saleType > 0);
    db[":f_id"] = id;
    db[":f_state"] = state;
    db[":f_saletype"] = saleType;
    db[":f_date"] = date.isValid() ? date : QDate::currentDate();
    db[":f_time"] = time.isValid() ? time : QTime::currentTime();
    db[":f_staff"] = staff;
    db[":f_partner"] = partner;
    db[":f_amount"] = amount;
    db[":f_discount"] = discount;
    db[":f_comment"] = comment;
    db[":f_payment"] = payment;
    db[":f_datefor"] = deliveryDate.isValid() ? deliveryDate : QDate::currentDate();
    db[":f_cashier"] = cashier;
    db[":f_hall"] = hall;
}

bool ODraftSale::write(C5Database &db, QString &err)
{
    bool n = id.toString().isEmpty();
    if (n) {
        id = db.uuid();
    }
    bind(db);
    if (n) {
        return insert(db, "o_draft_sale", err);
    } else {
        return getWriteResult(db, db.update("o_draft_sale", "f_id", _id()), err);
    }
}
