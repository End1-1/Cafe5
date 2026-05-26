#ifndef OTAX_H
#define OTAX_H

#include "c5dbrecord.h"

class OTax : public TableRecord
{
public:
    QString dept;
    QString firmName;
    QString address;
    QString devNum;
    QString serial;
    QString fiscal;
    QString receiptNumber;
    QString hvhh;
    QString fiscalMode;
    QString time;

    virtual bool getRecord(C5Database &db) override;
    virtual void bind(C5Database &db);
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // OTAX_H
