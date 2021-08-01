#include "dbdishpart2.h"

DbDishPart2::DbDishPart2() :
    DbData("d_part2")
{

}

int DbDishPart2::part1(int id)
{
    return get(id, "f_part").toInt();
}

int DbDishPart2::parentid(int id)
{
    return get(id, "f_parent").toInt();
}

QString DbDishPart2::adgcode(int id)
{
    return get(id, "f_adgcode").toString();
}
