#include "dbhalls.h"

DbHalls::DbHalls() :
    DbData("h_halls")
{
}

DbHalls::DbHalls(int id) :
    DbData(id)
{
}

int DbHalls::settings(int id)
{
    return get(id, "f_settings").toInt();
}
