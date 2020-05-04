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
    double fRetailPrice;
    double fWhosalePrice;
    int fTaxDept;
    QString fAdgCode;
    double fQty;
};

#endif // GOODS_H
