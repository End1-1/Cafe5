#include "cpartners.h"

CPartners &CPartners::queryRecordOfId(C5Database &db, const QVariant &id)
{
    db[":f_id"] = id;
    db.exec("select * from c_partners where f_id=:f_id");
    getRecord(db);
    return *this;
}

bool CPartners::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getInt("f_id");
    taxCode = db.getString("f_taxcode");
    taxName = db.getString("f_taxname");
    contact = db.getString("f_contact");
    info = db.getString("f_info");
    phone = db.getString("f_phone");
    email = db.getString("f_email");
    address = db.getString("f_address");
    return true;
}

bool CPartners::write(C5Database &db, QString &err)
{
    db[":f_taxcode"] = taxCode;
    db[":f_taxname"] = taxName;
    db[":f_contact"] = contact;
    db[":f_info"] = info;
    db[":f_phone"] = phone;
    db[":f_email"] = email;
    db[":f_address"] = address;
    if (id == 0) {
        return getWriteResult(db, db.insert("c_partners", id), err);
    } else {
        return getWriteResult(db, db.update("c_partners", "f_id", id), err);
    }
}
