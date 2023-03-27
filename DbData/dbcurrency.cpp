#include "dbcurrency.h"
#include "datadriver.h"

DbCurrency::DbCurrency() :
    DbData("e_currency")
{

}

DbCurrency::DbCurrency(int id) :
    DbData(id)
{

}

QString DbCurrency::currencyName()
{
    return dbcurrency->name(fId);
}
