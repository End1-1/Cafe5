#ifndef STORE_H
#define STORE_H

#include "storerecord.h"

class Store
{
public:
    Store(int id);
    void addRecord(const QDate &date, const QString &base, int store, const QString &scancode, int goods, double price, double qty);
    QList<StoreRecord> getRecords(const QString &sku);
    int fStore;

private:
    QHash<QString, QList<StoreRecord> > fGoods;
};

#endif // STORE_H
