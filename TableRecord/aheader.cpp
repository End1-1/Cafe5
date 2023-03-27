#include "aheader.h"

bool AHeader::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.isEmpty()) {
        id = db.uuid();
        u = false;
    }
    db[":f_id"] = id;
    db[":f_userid"] = userId;
    db[":f_state"] = state;
    db[":f_type"] = type;
    db[":f_operator"] = operator_;
    db[":f_date"] = date;
    db[":f_createDate"] = dateCreate;
    db[":f_createTime"] = timeCreate;
    db[":f_partner"] = partner;
    db[":f_amount"] = amount;
    db[":f_comment"] = comment;
    db[":f_currency"] = currency;
    if (u) {
        return getWriteResult(db, db.update("a_header", "f_id", id), err);
    } else {
        return getWriteResult(db, db.insert("a_header", false), err);
    }
}
