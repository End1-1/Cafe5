#include "removeshopsale.h"
#include "c5database.h"
#include "c5storedraftwriter.h"
#include <QSet>

RemoveShopSale::RemoveShopSale(const QStringList dbParams, QObject *parent) : QObject(parent),
    fDbParams(dbParams)
{

}

bool RemoveShopSale::remove(C5Database &db, const QString &id)
{
    QStringList storeRec;
    db[":f_header"] = id;
    db.exec("select f_storerec from o_goods where f_header=:f_header");
    while (db.nextRow()) {
        storeRec.append(db.getString(0));
    }
    QSet<QString> storeHeader;
    foreach (const QString &s, storeRec) {
        db[":f_id"] = s;
        db.exec("select f_document from a_store_draft where f_id=:f_id");
        if (db.nextRow()) {
            storeHeader.insert(db.getString(0));
        }
    }
    C5StoreDraftWriter dw(db);
    foreach (const QString &s, storeHeader) {
        if (!dw.outputRollback(db, s)) {

        }
        db[":f_document"] = s;
        db.exec("delete from a_store_draft where f_document=:f_document");
        db[":f_id"] = s;
        db.exec("delete from a_header_store where f_id=:f_id");
        db[":f_id"] = s;
        db.exec("delete from a_header where f_id=:f_id");
    }

    storeHeader.clear();
    db[":f_oheader"] = id;
    db.exec("select f_id from a_header_cash where f_oheader=:f_oheader");
    if (db.nextRow()) {
        storeHeader.insert(db.getString(0));
    }
    foreach (const QString &s, storeHeader) {
        db[":f_header"] = s;
        db.exec("delete from e_cash where f_header=:f_header");
        db[":f_id"] = s;
        db.exec("delete from a_header_cash where f_id=:f_id");
        db[":f_id"] = s;
        db.exec("delete from a_header where f_id=:f_id");
    }


    db[":f_order"] = id;
    db.exec("delete from b_clients_debts where f_order=:f_order");
    db.exec("delete from a_header where f_id=:f_id");
    db[":f_trsale"] = id;
    db.exec("select * from b_gift_card_history where f_trsale=:f_trsale");
    if (db.nextRow()) {
        if (db.getDouble("f_amount") > 0) {
            db[":f_id"] = db.getInt("f_card");
            db[":f_datesaled"] = QVariant();
            db.exec("update b_gift_card set f_datesaled=:f_datesaled where f_id=:f_id");
        }
    }
    db[":f_trsale"] = id;
    db.exec("delete from b_gift_card_history where f_trsale=:f_trsale");

    db.deleteFromTable("b_clients_debts", "f_order", id);

    db[":f_header"] = id;
    db.exec("delete from o_goods where f_header=:f_header");
    db[":f_id"] = id;
    db.exec("delete from o_header where f_id=:f_id");
    db[":f_id"] = id;

    return true;

}
