#include "datadriver.h"
#include "networktable.h"
#include "sqdriver.h"

void data_list(RawMessage &rm, Database &db, const QString &queryName)
{
    NetworkTable nt(rm, db);
    nt.execSQL(sq(queryName));
}

void data_list2(RawMessage &rm, Database &db, const QString &queryName)
{
    NetworkTable nt(rm, db);
    nt.execSQL(queryName);
}
