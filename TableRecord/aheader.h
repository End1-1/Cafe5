#ifndef AHEADER_H
#define AHEADER_H

#include "c5dbrecord.h"

class AHeader : public TableRecord
{
public:
    QString id;
    QString userId;
    int state = 0;
    int type = 0;
    int operator_ = 0;
    QDate date;
    QDate dateCreate;
    QTime timeCreate;
    int partner = 0;
    double amount = 0.0;
    QString comment;
    int currency = 0;

    virtual bool write(C5Database &db, QString &err) override;
};

#endif // AHEADER_H
