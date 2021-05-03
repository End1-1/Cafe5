#ifndef GOODS_H
#define GOODS_H

#include <QString>
#include <QVariant>

class Goods
{
public:
    Goods();

    QString fCode;
    QString fScanCode;
    QString fName;
    QString fUnit;
    int fUnitCode;
    double fRetailPrice;
    double fWhosalePrice;
    double fLastInputPrice;
    int fTaxDept;
    QString fAdgCode;
    double fQty;
    bool fIsService;
    bool fStoreInputBeforeSale;
    bool fWholeNumber;
    int fStoreId;
};
Q_DECLARE_METATYPE(Goods)

#endif // GOODS_H
