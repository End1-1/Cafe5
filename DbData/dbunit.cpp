#include "dbunit.h"
#include "datadriver.h"

DbUnit::DbUnit() :
    DbData("c_units")
{

}

DbUnit::DbUnit(int id) :
    DbData(id)
{

}

QString DbUnit::unitName()
{
    return name(fId);
}

double DbUnit::defaultQty()
{
    return dbunit->get(fId, "f_defaultqty").toDouble();
}
