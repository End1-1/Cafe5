#include "store.h"
#include "translator.h"
#include "sqdriver.h"
#include "networktable.h"

void store_checkqty(RawMessage &rm, Database &db, const QByteArray &in)
{
    QString barcode;
    quint32 currency;
    rm.readString(barcode, in);
    rm.readUInt(currency, in);
    db[":f_scancode"] = barcode;
    if (db.exec("select * from c_goods where f_scancode=:f_scancode") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(tr_am("Invalid scancode"));
        return;
    }
    int goodsid = db.integer("f_id");
    QString name = db.string("f_name");
    db[":f_goods"] = goodsid;
    db[":f_currency"] = currency;
    NetworkTable nt(rm, db);
    nt.execSQL(sq("sq_store_check_qty"));
    rm.putString(name);
}
