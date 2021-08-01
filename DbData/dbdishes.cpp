#include "dbdishes.h"
#include "datadriver.h"

DbDishes::DbDishes() :
    DbData("d_dish")
{

}

bool DbDishes::isHourlyPayment(int id)
{
    return get(id, "f_hourlypayment").toInt() > 0;
}

int DbDishes::part1(int id)
{
    return dbdishpart2->part1(part2(id));
}

int DbDishes::part2(int id)
{
    return get(id, "f_part").toInt();
}

QString DbDishes::part1name(int id)
{
    return dbdishpart1->name(part1(id));
}

QString DbDishes::part2name(int id)
{
    return dbdishpart2->name(part2(id));
}

int DbDishes::color(int id)
{
    return get(id, "f_color").toInt();
}

bool DbDishes::canService(int id)
{
    return get(id, "f_service").toInt() > 0;
}

bool DbDishes::canDiscount(int id)
{
    return get(id, "f_discount").toInt() > 0;
}

int DbDishes::group(int id)
{
    return get(id, "f_group").toInt();
}
