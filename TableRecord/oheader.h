#ifndef OHEADER_H
#define OHEADER_H

#include "c5dbrecord.h"
#include "bhistory.h"
#include "ogoods.h"

class OHeader: public TableRecord {
public:
    int hallId;
    QString prefix;
    int state;
    int hall;
    int table;
    QDate dateOpen;
    QDate dateClose;
    QDate dateCash;
    QTime timeOpen;
    QTime timeClose;
    int cashier;
    int staff;
    QString comment;
    int print;
    double amountTotal;
    double amountCash;
    double amountCard;
    double amountPrepaid;
    double amountBank;
    double amountIdram;
    double amountTelcell;
    double amountPayX;
    double amountDebt;
    double amountHotel;
    double amountOther;
    double amountCashIn;
    double amountChange;
    double amountService;
    double amountDiscount;
    double serviceFactor;
    double discountFactor;
    int bankCard;
    int shift;
    int source;
    int saleType;
    int partner;
    int currentStaff;
    int guests;
    int precheck;
    int currency;
    QString taxpayerTin;

    bool _printFiscal = false;

    OHeader();
    virtual void bind(C5Database &db) override;
    virtual bool getRecord(C5Database &db) override;
    virtual bool write(C5Database &db, QString &err) override;
    QString humanId();
    bool hasIdram();
    void countAmount(QVector<OGoods> &goods, BHistory &bhistory);
};

#endif // OHEADER_H
