#ifndef STORE_H
#define STORE_H

#include "rawmessage.h"
#include "database.h"

void store_checkqty(RawMessage &rm, Database &db, const QByteArray &in);

#endif // STORE_H
