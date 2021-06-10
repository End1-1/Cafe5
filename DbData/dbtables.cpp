#include "dbtables.h"

DbTables::DbTables() :
    DbData("h_tables")
{
}

QString DbTables::name(int id)
{
    return get(id, "f_name").toString();
}

int DbTables::hall(int id)
{
    return get(id, "f_hall").toInt();
}
