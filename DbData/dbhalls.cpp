#include "dbhalls.h"

DbHalls::DbHalls() :
    DbData("h_halls")
{

}

int DbHalls::settings(int id)
{
    return get(id, "f_settings").toInt();
}
