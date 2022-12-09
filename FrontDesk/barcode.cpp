#include "barcode.h"
#include <QDebug>

Barcode::Barcode()
{

}

int Barcode::ean13CheckSum(const QString &code)
{
    if (code.toLongLong() == 0) {
        return -1;
    }
    int len = code.length();
    if (len != 12) {
        return -1;
    }
    int odd = 0, even = 0;
    for (int i = 0; i < len; i++) {
        if (i % 2) {
            even += QString(code.at(i)).toInt();
        } else {
            odd += QString(code.at(i)).toInt();
        }
    }
    even *= 3;
    QString sum = QString::number(odd + even);
    sum = sum.at(sum.length() - 1);
    if (sum.toInt() == 0) {
        return 0;
    }
    return 10 - sum.toInt();
}

int Barcode::ean8CheckSum(const QString &code)
{
    if (code.toLongLong() == 0) {
        return -1;
    }
    int len = code.length();
    if (len != 7) {
        return -1;
    }
    int odd = 0, even = 0;
    for (int i = 0; i < len; i++) {
        if (i % 2) {
            even += QString(code.at(i)).toInt();
        } else {
            odd += QString(code.at(i)).toInt();
        }
    }
    odd *= 3;
    QString sum = QString::number(odd + even);
    sum = sum.at(sum.length() - 1);
    if (sum.toInt() == 0) {
        return 0;
    }
    return 10 - sum.toInt();
}

bool Barcode::isEan13(const QString &code)
{
    if (code.length() != 13) {
        return false;
    }
    int checksum = ean13CheckSum(code.left(12));
    if (checksum < 0) {
        return false;
    }
    return checksum == QString(code.at(code.length() - 1)).toInt();
}

bool Barcode::isEan8(const QString &code)
{
    if (code.length() != 8) {
        return false;
    }
    int checksum = ean8CheckSum(code.left(7));
    if (checksum < 0) {
        return false;
    }
    return checksum == QString(code.at(code.length() - 1)).toInt();
}
