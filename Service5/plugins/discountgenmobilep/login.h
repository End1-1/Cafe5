#ifndef LOGIN_H
#define LOGIN_H

#include "rawmessage.h"
#include "database.h"

class WaiterConnection;

int loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in, QString &username);
int loginPin(RawMessage &rm, Database &db, const QByteArray &in, QString &username);
void createQrDiscount(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w);
void checkBonuses(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w);

#endif // LOGIN_H
