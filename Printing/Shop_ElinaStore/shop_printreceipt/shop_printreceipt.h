#ifndef SHOP_PRINTRECEIPT_H
#define SHOP_PRINTRECEIPT_H

#include "shop_printreceipt_global.h"
#include <QObject>

class C5Database;

class SHOP_PRINTRECEIPTSHARED_EXPORT Shop_printreceipt : public QObject
{
    Q_OBJECT
public:
    Shop_printreceipt(C5Database *db, const QString &order);

    void print();

private:
    C5Database *fDb;

    QString fOrder;
};

#endif // SHOP_PRINTRECEIPT_H
