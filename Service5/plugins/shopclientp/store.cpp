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
    nt.execSQL("select sn.f_name as `Պահեստ`, g.f_name as `Ապրանք`, sum(s.f_qty*s.f_type) as `Քանակ`, "
               "gp.f_price1 as `Մանր․ գին`, gp.f_price2 as `Մեծ․ գին`, g.f_scancode as `Բարկոդ`,  g.f_id "
               "from a_store s "
               "left join c_goods g on g.f_id=s.f_goods "
               "left join c_goods_prices gp on gp.f_goods=s.f_goods "
               "left join c_storages sn on sn.f_id=s.f_store "
               "where s.f_goods=:f_goods and gp.f_currency=:f_currency "
               "group by 1 "
               "having sum(s.f_qty*s.f_type) > 0 ");
    rm.putString(name);
}
