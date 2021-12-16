#include "dbmenupackages.h"
#include "datadriver.h"

DbMenuPackages::DbMenuPackages() :
    DbData("d_package")
{

}

DbMenuPackages::DbMenuPackages(int id) :
    DbData(id)
{

}

QString DbMenuPackages::name()
{
    return dbmenupackage->get(fId, "f_name").toString();
}

double DbMenuPackages::price()
{
    return dbmenupackage->get(fId, "f_price").toDouble();
}

bool DbMenuPackages::isEnabled()
{
    return dbmenupackage->get(fId, "f_enabled").toInt() != 0;
}
