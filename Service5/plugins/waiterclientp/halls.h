#ifndef HALLS_H
#define HALLS_H

#include "rawmessage.h"
#include "database.h"

void loadHalls(RawMessage &rm, Database &db, const QByteArray &in);
void loadTables(RawMessage &rm, Database &db, const QByteArray &in);
void loadCarsModels(RawMessage &rm, Database &db, const QByteArray &in);
bool searchPlateNumber(RawMessage &rm, Database &db, const QByteArray &in);

#endif // HALLS_H
