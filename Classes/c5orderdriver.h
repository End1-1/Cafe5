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

    bool newOrder(int userid, QString &id, int tableId);

    bool closeOrder();

    bool loadData(const QString id);

    bool reloadOrder();

    bool setRoom(const QString &res, const QString &inv, const QString &room, const QString &guest);

    bool setCL(const QString &code, const QString &name);

    bool save();

    QString error() const;

    int ordersCount();

    int dishesCount();

    bool isEmpty();

    double amountTotal();

    double prepayment();

    C5OrderDriver &setCurrentOrderID(const QString &id);

    QString currentOrderId();

    C5OrderDriver &setHeader(const QString &key, const QVariant &value);

    QVariant headerValue(const QString &key);

    QVariant taxValue(const QString &key);

    C5OrderDriver &setHeaderOption(const QString &key, const QVariant &value);

    QVariant headerOptionsValue(const QString &key);

    C5OrderDriver &setPreorder(const QString &key, const QVariant &value);

    QVariant preorder(const QString &key);

    QVariant payRoomValue(const QString &key);

    QVariant clValue(const QString &key);

    const QMap<QString, QVariant> &dish(int index) const;

    bool addDish(int menuid, const QString &comment);

    bool addDish(QMap<QString, QVariant> o);

    void removeDish(int index);

    C5OrderDriver &setDishesValue(const QString &key, const QVariant &value, int index);

    QVariant dishesValue(const QString &key, int index);

    double dishTotal(int index);

    int duplicateDish(int index);

private:
    C5Database fDb;

    QString fLastError;

    QString fCurrentOrderId;

    int fTable;

    QMap<QString, QVariant> fHeader;

    QMap<QString, QVariant> fHeaderOptions;

    QMap<QString, QVariant> fHeaderPreorder;

    QMap<QString, QVariant> fTaxInfo;

    QMap<QString, QVariant> fPayRoom;

    QMap<QString, QVariant> fPayCL;

    QList<QMap<QString, QVariant> >  fDishes;

    QMap<QString, QMap<QString, QVariant> > fTableData;

    QMap<QString, QMap<QString, QVariant> > fDishesTableData;

    bool fetchTableData(const QString &sql, QMap<QString, QVariant> &data);

    bool fetchDishesData(const QString &header, const QString &id);

    void clearOrder();
};

#endif // C5ORDERDRIVER_H
