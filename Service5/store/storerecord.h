#ifndef STORERECORD_H
#define STORERECORD_H

#include <QDate>

struct StoreRecord
{
    QDate fDate;
    QString fBase;
    int fStore;
    int fGoods;
    QString fSku;
    double fPrice;
    double fQty;
    double fTotal;
    StoreRecord() {
        fDate = QDate::currentDate();
        fStore = 0;
        fGoods = 0;
        fPrice = 0;
        fQty = 0;
        fTotal = 0;
    }
};

#endif // STORERECORD_H
