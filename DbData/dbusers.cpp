#include "dbusers.h"

DbUsers::DbUsers() :
    DbData("s_user")
{
}

QString DbUsers::fullName(int id)
{
    return get(id, "f_last").toString() + " " + get(id, "f_first").toString();
}

QString DbUsers::fullShort(int id)
{
    return get(id, "f_last").toString().mid(0, 1) + ". " + get(id, "f_first").toString();
}
