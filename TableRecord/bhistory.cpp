#include "bhistory.h"

BHistory::BHistory()
{

}

bool BHistory::write(C5Database &db, QString &err)
{
    db[":f_id"] = id;
    db.exec("select * from b_history");
    bool u = db.nextRow();
    db[":f_id"] = id;
    db[":f_type"] = type;
    db[":f_card"] = card;
    db[":f_value"] = value;
    db[":f_data"] = data;
    if (u) {
        return getWriteResult(db, db.update("b_history", where_id(id)), err);
    } else {
        return getWriteResult(db, db.insert("b_history", false), err);
    }
}
