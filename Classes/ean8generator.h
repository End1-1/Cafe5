#ifndef EAN8GENERATOR_H
#define EAN8GENERATOR_H

#include <QImage>
#include <QString>

class Ean8Generator
{
public:
    static QImage generate(const QString& digits, int height = 100);
    static QString last(const QString& digits);
private:
    static QString calculateChecksum(const QString& digits);
    static QString encodeDigit(int digit, bool left);
};

#endif // EAN8GENERATOR_H
