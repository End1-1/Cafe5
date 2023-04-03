#include "otax.h"

bool OTax::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getString("f_id");
    dept = db.getString("f_debt");
    address = db.getString("f_address");
    firmName = db.getString("f_firmname");
    devNum = db.getString("f_devnum");
    serial = db.getString("f_serial");
    fiscal = db.getString("f_fiscal");
    receiptNumber = db.getString("f_receiptnumber");
    hvhh = db.getString("f_hvhh");
    fiscalMode = db.getString("f_fiscalmode");
    time = db.getString("f_time");
    return true;
}

void OTax::bind(C5Database &db)
{
    db[":f_id"] = id;
    db[":f_dept"] = dept;
    db[":f_address"] = address;
    db[":f_firmname"] = firmName;
    db[":f_devnum"] = devNum;
    db[":f_serial"] = serial;
    db[":f_fiscal"] = fiscal;
    db[":f_receiptnumber"] = receiptNumber;
    db[":f_hvhh"] = hvhh;
    db[":f_fiscalmode"] = fiscalMode;
    db[":f_time"] = time;
}

bool OTax::write(C5Database &db, QString &err)
{
    bool u = true;
    if (!id.toString().isEmpty()) {
        db[":f_id"] = id;
        db.exec("select * from o_tax where f_id=:f_id");
        u = db.nextRow();
    }

    if (u) {
        return update(db, "o_tax", err);
    } else {
        return insert(db, "o_tax", err);
    }
}
