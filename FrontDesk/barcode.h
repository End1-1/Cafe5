#ifndef BARCODE_H
#define BARCODE_H

#include <QString>

class Barcode
{
public:
    Barcode();
    QString Code_128(const QString &A) const;

private:
    QString Code_Char(const QString &A) const;
    QString Code_128_ID(int ID) const;
};

#endif // BARCODE_H
