#ifndef LOGIN_H
#define LOGIN_H

#include "rawmessage.h"
#include "database.h"

void loginUsernamePassword(RawMessage &rm, Database &db, const QByteArray &in);
void loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in);

#endif // LOGIN_H
