#include "hourlypay.h"
#include <QDebug>

double total(const QDateTime &start, QString &time)
{
    qDebug() << start.toString("dd/MM/yyyy HH:mm:ss");
    int sec = start.msecsTo(QDateTime::currentDateTime()) / 1000;
    int hours = (sec / 60) / 60;
    int min = (sec / 60) % 60;
    double amount = 5000;
    int hoursRemain = hours - 2;
    if (min > 0) {
        hoursRemain++;
    }
    if (hoursRemain < 0) {
        hoursRemain = 0;
    }
    time = QString("%1:%2").arg(hours, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0'));
    amount += (hoursRemain * 2000);
    return amount;
}
