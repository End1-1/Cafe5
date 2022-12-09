#ifndef LOGIN_H
#define LOGIN_H

#include "rawmessage.h"
#include "database.h"

int loginUsernamePassword(RawMessage &rm, Database &db, const QByteArray &in, QString &fullname);
int loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in, QString &username);
int loginPin(RawMessage &rm, Database &db, const QByteArray &in, QString &username);

#endif // LOGIN_H
