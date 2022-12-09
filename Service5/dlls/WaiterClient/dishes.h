#ifndef DISHES_H
#define DISHES_H

#include "rawmessage.h"
#include "database.h"

void loadPart1(RawMessage &rm, Database &db, const QByteArray &in);
void loadPart2(RawMessage &rm, Database &db, const QByteArray &in);

#endif // DISHES_H
