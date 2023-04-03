#include "bclientdebts.h"

QString BClientDebts::currencyName(C5Database &db) const
{
    db[":f_id"] = currency;
    db.exec("select f_name from e_currency where f_id=:f_id");
    db.nextRow();
    return db.getString("f_name");
}

bool BClientDebts::getRecord(C5Database &db)
{
    if (!db.nextRow()) {
        return false;
    }
    id = db.getInt("f_id");
    source = db.getInt("f_source");
    date = db.getDate("f_date");
    costumer = db.getInt("f_costumer");
    order = db.getString("f_order");
    cash = db.getString("f_cash");
    store = db.getString("f_storedoc");
    amount = db.getDouble("f_amount");
    currency = db.getInt("f_currency");
}

void BClientDebts::bind(C5Database &db)
{
    db[":f_id"] = id;
    db[":f_source"] = source;
    db[":f_date"] = date;
    db[":f_costumer"] = costumer;
    db[":f_order"] = order;
    db[":f_cash"] = cash;
    db[":f_storedoc"] = store;
    db[":f_amount"] = amount;
    db[":f_currency"] = currency;
}

bool BClientDebts::write(C5Database &db, QString &err)
{
    Q_ASSERT(source > 0);
    bool u = true;
    if (id == 0) {
        u = false;
    }
    bind(db);
    if (u) {
        return getWriteResult(db, db.update("b_clients_debts", "f_id", id), err);
    } else {
        return getWriteResult(db, db.insert("b_clients_debts", id), err);
    }
}
