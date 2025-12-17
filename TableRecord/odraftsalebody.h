
#ifndef ODRAFTSALEBODY_H
#define ODRAFTSALEBODY_H

#include "c5dbrecord.h"

class ODraftSaleBody : public TableRecord
{
public:
    QString header;
    int state = 0;
    int store = 0;
    QDate dateAppend;
    QTime timeAppend;
    int goods = 0;
    double qty = 0.0;
    double back = 0.0;
    double price = 0.0;
    double discount = 0.0;
    QDate dateRemoved;
    QTime timeRemoved;
    int userAppend = 0;
    int userRemove = 0;

    virtual bool getRecord(C5Database &db) override;
    virtual void bind(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // ODRAFTSALEBODY_H
