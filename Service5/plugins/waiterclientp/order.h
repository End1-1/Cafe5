#ifndef ORDER_H
#define ORDER_H

#include "rawmessage.h"
#include "database.h"

class WaiterConnection;

void openTable(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *wc);
void unlockTable(RawMessage &rm, Database &db, const QByteArray &in, const QString &session, WaiterConnection *wc);
void forceUnlockTable(Database &db, const QString &sessionid, int version);
void createHeaderOfOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid);
void createHeaderOfOrderV2(RawMessage &rm, Database &db, const QByteArray &in, int userid, int tableid);
void setCar(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w);
void getCar(RawMessage &rm, Database &db, const QByteArray &in);
void openOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid);
void addDishToOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid);
void removeDishFromOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid);
void modifyDishOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid);
void printService(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w);
bool printServiceOnPrinter(const QMap<QString, QVariant> &order, const QMap<QString, QList<int> > &dishesToPrint, const QList<QMap<QString, QVariant> > &dishes);
bool countOrder1(RawMessage &rm, Database &db, const QString &orderid);
bool countOrder2(RawMessage &rm, Database &db, const QString &orderid);
void printBill(RawMessage &rm, Database &db, const QByteArray &in, int userid);
bool printBill1(RawMessage &rm, Database &db, const QString &orderid);
bool printBill2(RawMessage &rm, Database &db, const QString &orderid);
bool printTax1(RawMessage &rm, Database &db, const QString &orderid);
void readyDishes(RawMessage &rm, Database &db, const QByteArray &in);

#endif // ORDER_H
