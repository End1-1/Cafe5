#include "aheader.h"

bool AHeader::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getString("f_id");
    return true;
}

void AHeader::bind(C5Database &db)
{
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
}

bool AHeader::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.isEmpty()) {
        id = db.uuid();
        u = false;
    }
    bind(db);
    if (u) {
        return getWriteResult(db, db.update("a_header", "f_id", id), err);
    } else {
        return getWriteResult(db, db.insert("a_header", false), err);
    }
}
