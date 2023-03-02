#ifndef PRINTBILL_H
#define PRINTBILL_H

#include <QMap>
#include <QVariant>

class Database;

class PrintBill
{
public:
    PrintBill();
    bool printBill2(Database *d, bool printFiscal, const QString &orderid, QString printerName, int language, QString &err);
    int printTax(Database &db, QMap<QString, QVariant> &header, QList<QMap<QString, QVariant> > &body, QString &err);
};

#endif // PRINTBILL_H
