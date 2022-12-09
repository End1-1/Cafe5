#ifndef ORDER_H
#define ORDER_H

#include "rawmessage.h"
#include "database.h"

void openTable(RawMessage &rm, Database &db, const QByteArray &in);
void unlockTable(RawMessage &rm, Database &db, const QByteArray &in);
void createHeaderOfOrder(RawMessage &rm, Database &db, const QByteArray &in);

#endif // ORDER_H
