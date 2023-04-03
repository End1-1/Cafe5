#include "odraftsalebody.h"

bool ODraftSaleBody::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getString("f_id");
    header = db.getString("f_header");
    state = db.getInt("f_state");
    store = db.getInt("f_store");
    dateAppend = db.getDate("f_dateappend");
    timeAppend = db.getTime("f_timeappend");
    goods = db.getInt("f_goods");
    qty = db.getDouble("f_qty");
    back = db.getDouble("f_back");
    dateRemoved = db.getDate("f_dateremoved");
    timeRemoved = db.getTime("f_timeremoved");
    userAppend = db.getInt("f_userappend");
    userRemove = db.getInt("f_userremove");
    return true;
}

void ODraftSaleBody::bind(C5Database &db)
{
    db[":f_id"] = id;
    db[":f_header"] = header;
    db[":f_state"] = state;
    db[":f_store"] = store;
    db[":f_dateappend"] = dateAppend;
    db[":f_timeappend"] = timeAppend;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_back"] = back;
    db[":f_price"] = price;
    db[":f_dateRemoved"] = dateRemoved;
    db[":f_timeRemoved"] = timeRemoved;
    db[":f_userAppend"] = userAppend;
    db[":f_userRemove"] = userRemove;
}

bool ODraftSaleBody::write(C5Database &db, QString &err)
{
    bool n = id.isEmpty();
    if (n) {
        id = db.uuid();
    }
    bind(db);
    if (n) {
        return getWriteResult(db, db.insert("o_draft_sale_body", false), err);
    } else {
        return getWriteResult(db, db.update("o_draft_sale_body", "f_id", id), err);
    }
    return true;
}
