#ifndef CPARTNERS_H
#define CPARTNERS_H

#include "c5dbrecord.h"

class CPartners : public TableRecord
{
public:
    int id;
    QString taxCode;
    QString taxName;
    QString contact;
    QString email;
    QString info;
    QString phone;
    QString address;

    virtual CPartners &queryRecordOfId(C5Database &db, const QVariant &id) override;
    virtual bool getRecord(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // CPARTNERS_H
