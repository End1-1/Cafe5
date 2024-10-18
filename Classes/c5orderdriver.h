#ifndef C5ORDERDRIVER_H
#define C5ORDERDRIVER_H

#include "c5storedraftwriter.h"
#include <QObject>
#include <QMap>
#include <QVariant>

#define va(x) QVariant(x)

class C5OrderDriver : public QObject
{
    Q_OBJECT

public:
    C5OrderDriver(const QStringList &dbParams);

    bool closeOrder();

    void setCloseHeader();

    bool setRoom(const QString &res, const QString &inv, const QString &room, const QString &guest);

    bool setCL(const QString &code, const QString &name);

    void fromJson(const QJsonObject &jdoc);

    bool save();

    QString error() const;

    int ordersCount();

    int dishesCount();

    bool isEmpty();

    double amountTotal();

    double clearAmount();

    double prepayment();

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

    QVariant headerHotel(const QString &key);

    C5OrderDriver &setHeaderHotel(const QString &key, const QVariant &value);

    const QMap<QString, QVariant> &dish(int index) const;

    bool addDish(int menuid, const QString &comment, double price, const QString &emark);

    bool addDish2(int packageid, double qty);

    bool addDish(QMap<QString, QVariant> o);

    void removeDish(int index);

    C5OrderDriver &setDishesValue(const QString &key, const QVariant &value, int index);

    QVariant dishesValue(const QString &key, int index);

    double dishTotal(int index);

    double dishTotal2(int index);

    int duplicateDish(int index);

    QMap<QString, QVariant> fHeader;

    QMap<QString, QVariant> fHeaderOptions;

    QMap<QString, QVariant> fHeaderPreorder;

    QMap<QString, QVariant> fTaxInfo;

    QMap<QString, QVariant> fPayRoom;

    QMap<QString, QVariant> fPayCL;

    QMap<QString, QVariant> fHeaderHotel;

    QList<QMap<QString, QVariant> >  fDishes;

    QMap<QString, QMap<QString, QVariant> > fTableData;

    QMap<QString, QMap<QString, QVariant> > fDishesTableData;

private:
    QStringList fDbParams;

    QString fLastError;

    QString fCurrentOrderId;

    int fTable;

    void clearOrder();

    void dateCash(QDate &d, int &dateShift);

    void writeCashDoc(C5StoreDraftWriter &dw, const QString &uuid, const QString id, QString &err, double amount, int staff,
                      int cashboxid, QDate dateCash);
};

#endif // C5ORDERDRIVER_H
