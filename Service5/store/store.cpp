#include "store.h"

Store::Store(int id) :
    fStore(id)
{

}

void Store::addRecord(const QDate &date, const QString &base, int store, const QString &scancode, int goods, double price, double qty)
{
    StoreRecord g;
    g.fDate = date;
    g.fBase = base;
    g.fStore = store;
    g.fSku = scancode;
    g.fGoods = goods;
    g.fPrice = price;
    g.fQty = qty;
    g.fTotal = qty * price;
    if (!fGoods.contains(scancode)) {
        fGoods.insert(scancode, QList<StoreRecord>());
    }
    QList<StoreRecord> &gl = fGoods[scancode];
    gl.append(g);
}

QList<StoreRecord> Store::getRecords(const QString &sku)
{
    if (fGoods.contains(sku)) {
        return fGoods[sku];
    }
    return QList<StoreRecord>();
}
