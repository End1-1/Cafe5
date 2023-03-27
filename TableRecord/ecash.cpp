#include "ecash.h"

bool ECash::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.isEmpty()) {
        id = db.uuid();
        u = false;
    }
    db[":f_id"] = id;
    db[":f_cash"] = cash;
    db[":f_sign"] = sign;
    db[":f_header"] = header;
    db[":f_remarks"] = remarks;
    db[":f_amount"] = amount;
    db[":f_base"] = base;
    db[":f_row"] = row;
    if (u) {
        return getWriteResult(db, db.update("e_cash", "f_id", id), err);
    } else {
        return getWriteResult(db, db.insert("e_cash", false), err);
    }
}
