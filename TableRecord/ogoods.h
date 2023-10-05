#ifndef OGOODS_H
#define OGOODS_H

#include "c5dbrecord.h"

class OGoods: public TableRecord {
public:
    QString header;
    QString body;
    int store = 0;
    int goods = 0;
    double qty = 0.0;
    double price = 0.0;
    double total = 0.0;
    double discountAmount = 0.0;
    double accumulateAmount = 0.0;
    int tax = 0;
    int sign = 1;
    QString taxDept;
    QString adgCode;
    int row = 0;
    QString storeRec;
    double discountFactor = 0.0;
    int discountMode = 0;
    int return_ = 0;
    QString returnFrom;
    int isService = 0;

    QString _groupName;
    QString _goodsName;
    QString _barcode;
    QString _unitName;
    double _qtybox = 0.0;
    QString emarks;

    virtual void bind(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
    virtual bool getRecord(C5Database &db) override;

};

#endif // OGOODS_H
