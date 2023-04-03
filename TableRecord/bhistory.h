#ifndef BHISTORY_H
#define BHISTORY_H

#include "c5dbrecord.h"

class BHistory : public TableRecord
{
public:
    QString id;
    int type = 0;
    int card = 0;
    double value = 0.0;
    double data = 0.0;
    BHistory();

    virtual void bind(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // BHISTORY_H
