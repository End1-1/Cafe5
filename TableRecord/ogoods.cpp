#include "ogoods.h"

bool OGoods::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.isEmpty()) {
        id = db.uuid();
        u = false;
    }
    db[":f_id"] = id;
    db[":f_header"] = header;
    db[":f_body"] = body;
    db[":f_store"] = store;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_price"] = price;
    db[":f_total"] = total;
    db[":f_tax"] = tax;
    db[":f_taxdept"] = taxDept;
    db[":f_adgcode"] = adgCode;
    db[":f_sign"] = sign;
    db[":f_row"] = row;
    db[":f_storerec"] = storeRec;
    db[":f_discountfactor"] = discountFactor;
    db[":f_discountmode"] = discountMode;
    db[":f_discountamount"] = discountAmount;
    db[":f_amountAccumulate"] = accumulateAmount;
    db[":f_return"] = return_;
    db[":f_returnfrom"] = returnFrom;
    db[":f_isservice"] = isService;
    if (u) {
        return getWriteResult(db, db.update("o_goods", where_id(id)), err);
    } else {
        return getWriteResult(db, db.insert("o_goods", false), err);
    }
}

bool OGoods::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
}
