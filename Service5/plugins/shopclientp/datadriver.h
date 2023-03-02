#ifndef DATADRIVER_H
#define DATADRIVER_H

#include "rawmessage.h"
#include "database.h"

void data_list(RawMessage &rm, Database &db, const QString &queryName);
void data_list2(RawMessage &rm, Database &db, const QString &queryName);

#endif // DATADRIVER_H
