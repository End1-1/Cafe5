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
    int print = 0;
    double amountTotal = 0;
    double amountCash = 0;
    double amountCard = 0;
    double amountPrepaid = 0;
    double amountBank = 0;
    double amountCredit = 0;
    double amountIdram = 0;
    double amountTelcell = 0;
    double amountPayX = 0;
    double amountDebt = 0;
    double amountHotel = 0;
    double amountOther = 0;
    double amountCashIn = 0;
    double amountChange = 0;
    double amountService = 0;
    double amountDiscount = 0;
    double serviceFactor = 0;
    double discountFactor = 0;
    int bankCard = 0;
    int shift = 0;
    int source = 0;
    int saleType = 0;
    int partner = 0;
    int currentStaff = 0;
    int guests = 0;
    int precheck = 0;
    int currency = 0;
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
