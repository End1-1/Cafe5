#include "cpartners.h"

CPartners &CPartners::queryRecordOfId(C5Database &db, const QVariant &id)
{
    db[":f_id"] = id;
    db.exec("select p.*, pc.f_name as f_categoryname, ps.f_name as f_statename, pg.f_name as f_groupname "
            "from c_partners p "
            "left join c_partners_state ps on ps.f_id=p.f_state "
            "left join c_partners_group pg on pg.f_id=p.f_group "
            "left join c_partners_category pc on pc.f_id=p.f_category "
            "where p.f_id=:f_id");
    getRecord(db);
    return *this;
}

bool CPartners::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getInt("f_id");
    category= db.getInt("f_category");
    categoryName = db.getString("f_categoryname");
    state = db.getInt("f_state");
    stateName = db.getString("f_statename");
    group = db.getInt("f_group");
    groupName = db.getString("f_groupname");
    taxCode = db.getString("f_taxcode");
    taxName = db.getString("f_taxname");
    contact = db.getString("f_contact");
    info = db.getString("f_info");
    phone = db.getString("f_phone");
    email = db.getString("f_email");
    address = db.getString("f_address");
    legalAddress = db.getString("f_legaladdress");
    permanentDiscount = db.getDouble("f_permanent_discount");
    pricePolitic = db.getInt("f_price_politic");
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
    db[":f_permanent_discount"] = permanentDiscount;
    db[":f_price_politic"] = pricePolitic;
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
