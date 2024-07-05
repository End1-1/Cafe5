#include "ogoods.h"

void OGoods::bind(C5Database &db)
{
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
    db[":f_emarks"] = emarks;
}

bool OGoods::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.toString().isEmpty()) {
        id = db.uuid();
        u = false;
    }
    bind(db);
    if (u) {
        return getWriteResult(db, db.update("o_goods", where_id(id.toString())), err);
    } else {
        return getWriteResult(db, db.insert("o_goods", false), err);
    }
}

bool OGoods::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getString("f_id");
    header = db.getString("f_header");
    body = db.getString("f_body");
    store = db.getInt("f_store");
    goods = db.getInt("f_goods");
    qty = db.getDouble("f_qty");
    price = db.getDouble("f_price");
    total = qty *price;
    tax = db.getInt("f_tax");
    sign = db.getInt("f_sign");
    discountFactor = db.getDouble("f_discountfactor");
    discountMode = db.getInt("f_discountmode");
    taxDept = db.getInt("f_taxdept");
    adgCode = db.getString("f_adgcode");
    return_ = db.getInt("f_return");
    returnFrom = db.getString("f_returnfrom");
    isService = db.getInt("f_isservice");
    storeRec = db.getString("f_storerec");
    emarks = db.getString("f_emarks");
    return true;
}
