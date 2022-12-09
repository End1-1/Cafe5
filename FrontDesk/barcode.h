#ifndef BARCODE_H
#define BARCODE_H

#include <QString>

class Barcode
{
public:
    Barcode();

    int ean13CheckSum(const QString &code);

    int ean8CheckSum(const QString &code);

    bool isEan13(const QString &code);

    bool isEan8(const QString &code);

};

#endif // BARCODE_H
