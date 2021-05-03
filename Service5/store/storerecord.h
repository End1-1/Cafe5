#ifndef STORERECORD_H
#define STORERECORD_H

#include <QDate>

struct StoreRecord
{
    QDate fDate;
    QString fBase;
    int fStore;
    int fGoods;
    double fPrice;
    double fQty;
    double fTotal;
};

#endif // STORERECORD_H
