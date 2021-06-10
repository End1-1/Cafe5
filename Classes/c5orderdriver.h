#ifndef C5ORDERDRIVER_H
#define C5ORDERDRIVER_H

#include "c5database.h"
#include <QObject>

#define va(x) QVariant(x)

class C5OrderDriver : public QObject
{
    Q_OBJECT

public:
    C5OrderDriver(const QStringList &dbParams);

    bool openTable(int table);

    bool loadData(const QString &id);

    QString error() const;

    int ordersCount();

    C5OrderDriver setCurrentOrderID(const QString &id);

    C5OrderDriver setHeader(const QString &key, const QVariant &value);

private:
    C5Database fDb;

    QString fLastError;

    QString fCurrentOrderId;

    int fTable;

    QMap<QString, QMap<QString, QVariant> > fHeader;

    QMap<QString, QMap<QString, QVariant> > fTaxInfo;

    QMap<QString, QList<QMap<QString, QVariant> > > fDishes;

    QMap<QString, QMap<QString, QVariant> > fTableData;
};

#endif // C5ORDERDRIVER_H
