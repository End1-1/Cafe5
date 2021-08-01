#include "dbmenu.h"

DbMenu::DbMenu() :
    DbData("d_menu", "f_state=1")
{

}

int DbMenu::menuid(int id)
{
    return get(id, "f_menu").toInt();
}

int DbMenu::dishid(int id)
{
    return get(id, "f_dish").toInt();
}

bool DbMenu::recent(int id)
{
    return get(id, "f_recent").toInt() > 0;
}

double DbMenu::price(int id)
{
    return get(id, "f_price").toDouble();
}

int DbMenu::store(int id)
{
    return get(id, "f_store").toInt();
}

QString DbMenu::print1(int id)
{
    return get(id, "f_print1").toString();
}

QString DbMenu::print2(int id)
{
    return get(id, "f_print2").toString();
}
