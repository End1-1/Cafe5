#include "dbdishes.h"

DbDishes::DbDishes() :
    DbData("d_dish")
{

}

bool DbDishes::isHourlyPayment(int id)
{
    return get(id, "f_hourlypayment").toInt() > 0;
}
