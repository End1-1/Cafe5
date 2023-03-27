#ifndef ECASH_H
#define ECASH_H

#include "c5dbrecord.h"

class ECash : public TableRecord
{
public:
    QString id;
    QString header;
    int cash;
    int sign;
    double amount;
    QString remarks;
    QString base;
    int row;

    virtual bool write(C5Database &db, QString &err) override;
};

#endif // ECASH_H
