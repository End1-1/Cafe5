#ifndef STORE_H
#define STORE_H

#include "storerecord.h"

class Store
{
public:
    Store(int id);
    void addRecord(const QDate &date, const QString &base, int goods, double price, double qty);

private:
    int fStore;
};

#endif // STORE_H
