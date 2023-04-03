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
    state = db.getInt("f_state");
    group = db.getInt("f_group");
    taxCode = db.getString("f_taxcode");
    taxName = db.getString("f_taxname");
    contact = db.getString("f_contact");
    info = db.getString("f_info");
    phone = db.getString("f_phone");
    email = db.getString("f_email");
    address = db.getString("f_address");
    legalAddress = db.getString("f_legaladdress");
    return true;
}

void CPartners::bind(C5Database &db)
{
    db[":f_state"] = state;
    db[":f_group"] = group;
    db[":f_taxcode"] = taxCode;
    db[":f_taxname"] = taxName;
    db[":f_contact"] = contact;
    db[":f_info"] = info;
    db[":f_phone"] = phone;
    db[":f_email"] = email;
    db[":f_address"] = address;
    db[":f_legaladdress"] = legalAddress;
}

bool CPartners::write(C5Database &db, QString &err)
{
    bind(db);
    if (id.toInt() == 0) {
        return insertWithId(db, "c_partners", err);
    } else {
        return update(db, "c_partners", err);
    }
}
