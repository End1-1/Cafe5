#ifndef C5WAITERORDERDOC_H
#define C5WAITERORDERDOC_H

#include "c5database.h"
#include <QJsonObject>
#include <QJsonArray>

class C5SocketHandler;

class C5WaiterOrderDoc : public QObject
{
    Q_OBJECT

public:
    C5WaiterOrderDoc();

    C5WaiterOrderDoc(const QString &id, C5Database &db);

    C5WaiterOrderDoc(C5Database &db, QJsonObject &jh, QJsonArray &jb);

    ~C5WaiterOrderDoc();

    bool isEmpty();

    int itemsCount();

    QJsonObject item(int index);

    void addItem(const QJsonObject &obj);

    void sendToServer(C5SocketHandler *sh);

    bool transferToHotel(C5Database &db, C5Database &fDD, QString &err);

    bool makeOutputOfStore(C5Database &db, QString &err);

    static void removeDocument(C5Database &db, const QString &id);

    QJsonObject fHeader;

    QJsonObject fTax;

    QJsonArray fItems;

    int hInt(const QString &name);

    double hDouble(const QString &name);

    QString hString(const QString &name);

    void hSetString(const QString &name, const QString &value);

    void hSetInt(const QString &name, int value);

    void hSetDouble(const QString &name, double value);

    int iInt(const QString &name, int index);

    double iDouble(const QString &name, int index);

    QString iString(const QString &name, int index);

    void iSetString(const QString &name, const QString &value, int index);

    void iSetInt(const QString &name, int value, int index);

    void iSetDouble(const QString &name, double value, int index);

    QString prepayment();

    void countTotal();

    void calculateSelfCost(C5Database &db);

private:

    bool fSaved;

    void open(C5Database &db);

    void getTaxInfo(C5Database &db);

    void countTotalV1();

    void countTotalV2();

    double countPreTotalV1();

    double countPreTotalV2();

    QString getHotelID(C5Database &db, const QString &source, QString &err);

    bool correctHotelID(QString &id, C5Database &dba, QString &err);
};

#endif // C5WAITERORDERDOC_H
