#ifndef BCLIENTDEBTS_H
#define BCLIENTDEBTS_H

#include "c5dbrecord.h"

#define BCLIENTDEBTS_SOURCE_SALE 1
#define BCLIENTDEBTS_SOURCE_INPUT 2

class BClientDebts : public TableRecord
{
public:
    int id = 0;
    QDate date;
    int source = 0;
    int costumer = 0;
    QString order;
    QString cash;
    QString store;
    double amount = 0;
    int currency = 0;

    QString currencyName(C5Database &db) const;
    virtual bool getRecord(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // BCLIENTDEBTS_H
