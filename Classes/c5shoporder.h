#ifndef C5SHOPORDER_H
#define C5SHOPORDER_H

#include "c5database.h"
#include "c5storedraftwriter.h"
#include "c5dbrecord.h"
#include "oheader.h"
#include "bhistory.h"
#include "ogoods.h"

class C5User;

class C5ShopOrder : public QObject
{
    Q_OBJECT

public:
    C5ShopOrder(OHeader &oheader, BHistory &bhistory, QVector<OGoods>& ogoods);

    bool write();

    bool writeFlags(int f1, int f2, int f3, int f4, int f5);

    bool returnFalse(const QString &msg, C5Database &db);

    bool fWriteAdvance;

    bool writeCash(C5Database &db, double value, int cash);

private:
    OHeader& fOHeader;

    BHistory& fBHistory;

    QVector<OGoods>& fOGoods;
};

#endif // C5SHOPORDER_H
