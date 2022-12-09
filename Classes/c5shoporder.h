#ifndef C5SHOPORDER_H
#define C5SHOPORDER_H

#include "c5database.h"
#include "c5storedraftwriter.h"

class C5User;

class C5ShopOrder : public QObject
{
    Q_OBJECT

public:
    C5ShopOrder(C5User *user);

    void setPayment(double cash, double change, bool debt, int currency);

    void setPartner(int partnerCode, const QString &partnerName);

    void setDiscount(int cardid, int cardmode, double cardvalue);

    void setParams(const QDate &dateOpen, const QTime &timeOpen, int saletype);

    bool write(double total, double card, double prepaid, double discount, bool tax, QList<IGoods> goods, double fDiscountFactor, int discmode, bool idram);

    bool writeFlags(int f1, int f2, int f3, int f4, int f5);

    bool returnFalse(const QString &msg, C5Database &db);

    QString fHeader;

    QString fHallId;

private:
    QDate fDateOpen;

    QTime fTimeOpen;

    int fSaleType;

    int fPartnerCode;

    int fCurrency;

    QString fPartnerName;

    int fCardId;

    int fCardMode;

    double fCardValue;

    double fCash;

    double fChange;

    C5User *fUser;

    bool fDebt;
};

#endif // C5SHOPORDER_H
