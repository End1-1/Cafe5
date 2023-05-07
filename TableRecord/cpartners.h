#ifndef CPARTNERS_H
#define CPARTNERS_H

#include "c5dbrecord.h"

class CPartners : public TableRecord
{
public:
    int category = 0;
    QString categoryName;
    int state = 0;
    QString stateName;
    int group = 0;
    QString groupName;
    QString taxCode;
    QString taxName;
    QString contact;
    QString email;
    QString info;
    QString phone;
    QString address;
    QString legalAddress;
    int pricePolitic = 0;
    double permanentDiscount = 0.0;

    virtual CPartners &queryRecordOfId(C5Database &db, const QVariant &id) override;
    virtual bool getRecord(C5Database &db) override;
    virtual void bind(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
};

#endif // CPARTNERS_H
