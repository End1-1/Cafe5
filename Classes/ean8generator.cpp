#include "ean8generator.h"
#include <QPainter>
#include <QColor>

static const char* LEFT_CODES[10] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

static const char* RIGHT_CODES[10] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

QString Ean8Generator::calculateChecksum(const QString& digits)
{
    int sum = 0;

    for(int i = 0; i < 7; ++i) {
        int d = digits.mid(i, 1).toInt();
        sum += (i % 2 == 0) ? d * 3 : d;
    }

    int checksum = (10 - (sum % 10)) % 10;
    return QString::number(checksum);
}

QString Ean8Generator::encodeDigit(int digit, bool left)
{
    return left ? LEFT_CODES[digit] : RIGHT_CODES[digit];
}

QImage Ean8Generator::generate(const QString& digits, int height)
{
    QString code = digits;

    if(code.size() < 7)
        return QImage();

    // добавляем контрольную цифру, если нет
    if(code.size() == 7)
        code += calculateChecksum(code);

    // формируем полосу штрихов
    QString bits = "101"; // start guard

    for(int i = 0; i < 4; ++i)
        bits += encodeDigit(code[i].digitValue(), true);

    bits += "01010"; // center guard

    for(int i = 4; i < 8; ++i)
        bits += encodeDigit(code[i].digitValue(), false);

    bits += "101"; // end guard
    // Рисуем штрихкод
    const int barWidth = 2;
    QImage img(bits.size() * barWidth, height, QImage::Format_RGB32);
    img.fill(Qt::white);
    QPainter p(&img);
    p.setPen(Qt::NoPen);

    for(int i = 0; i < bits.size(); ++i) {
        if(bits[i] == '1') {
            p.setBrush(Qt::black);
            p.drawRect(i * barWidth, 0, barWidth, height);
        }
    }

    p.end();
    return img;
}

QString Ean8Generator::last(const QString &digits)
{
    if(digits.size() < 7)
        return QString(); // мало символов

    QString base = digits.left(7);
    QString checksum = calculateChecksum(base);

    if(digits.size() == 7)
        return base + checksum;
    else
        return base + checksum;
}
