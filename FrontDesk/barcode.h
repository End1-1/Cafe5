#ifndef BARCODE_H
#define BARCODE_H

#include <QString>

class Barcode
{
public:
    Barcode();
    QString Code_128(const QString &A) const;
    QString Code_Interlived_2_5(const QString &A, bool Check = false) const;

private:
    QString Code_Char(const QString &A) const;
    QString Code_128_ID(int ID) const;
    QString Interleaved_2of5_Pair(const QString &Pair) const;
    QString Code_2of5_Ch(const QString &Ch) const;
};

#endif // BARCODE_H
