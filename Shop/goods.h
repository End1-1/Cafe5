#ifndef GOODS_H
#define GOODS_H

#include <QString>

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
    int fTaxDept;
    QString fAdgCode;
    double fQty;
    bool fIsService;
    int fUncomplectFrom;
    double fUncomplectQty;
};

#endif // GOODS_H
