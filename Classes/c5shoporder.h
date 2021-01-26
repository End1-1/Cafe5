#ifndef C5SHOPORDER_H
#define C5SHOPORDER_H

#include "c5database.h"
#include "c5storedraftwriter.h"

class C5ShopOrder : public QObject
{
    Q_OBJECT

public:
    C5ShopOrder();

    void setPayment(double cash, double change);

    void setPartner(int partnerCode, const QString &partnerName);

    void setDiscount(int customer, int cardid, int cardmode, double cardvalue);

    void setParams(const QDate &dateOpen, const QTime &timeOpen, int saletype);

    bool write(double total, double card, double prepaid, double discount, bool tax, QList<IGoods> goods, double fDiscountFactor, int discmode);

    bool returnFalse(const QString &msg, C5Database &db);

    QString fHeader;

    QString fHallId;

private:
    QDate fDateOpen;

    QTime fTimeOpen;

    int fSaleType;

    int fPartnerCode;

    QString fPartnerName;

    int fCustomerId;

    int fCardId;

    int fCardMode;

    double fCardValue;

    double fCash;

    double fChange;
};

#endif // C5SHOPORDER_H
