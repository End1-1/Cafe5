#ifndef ODRAFTSALE_H
#define ODRAFTSALE_H

#include "c5dbrecord.h"

class ODraftSale : public TableRecord
{
public:
    int state = 0;
    int saleType = 0;
    int hall = 0;
    QDate date;
    QTime time;
    int payment = 0;
    int staff = 0;
    int cashier = 0;
    int partner = 0;
    double amount = 0.0;
    double discount = 0.0;
    QString comment;
    QDate deliveryDate;

    virtual bool getRecord(C5Database &db) override;
    virtual void bind(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // ODRAFTSALE_H
