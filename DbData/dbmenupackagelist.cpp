#include "dbmenupackagelist.h"
#include "datadriver.h"

DbMenuPackageList::DbMenuPackageList() :
    DbData("d_package_list")
{

}

DbMenuPackageList::DbMenuPackageList(int id) :
    DbData(id)
{

}

int DbMenuPackageList::package()
{
    return dbmenupackagelist->get(fId, "f_package").toInt();
}

int DbMenuPackageList::dish()
{
    return dbmenupackagelist->get(fId, "f_dish").toInt();
}

double DbMenuPackageList::qty()
{
    return dbmenupackagelist->get(fId, "f_qty").toDouble();
}

double DbMenuPackageList::price()
{
    return dbmenupackagelist->get(fId, "f_price").toDouble();
}

int DbMenuPackageList::store()
{
    return dbmenupackagelist->get(fId, "f_store").toInt();
}

QString DbMenuPackageList::print1()
{
    return dbmenupackagelist->get(fId, "f_printer").toString();
}

QString DbMenuPackageList::print2()
{
    return "";
}

QList<int> DbMenuPackageList::listOf(int id)
{
    QList<int> lst;
    for (int idp: dbmenupackagelist->list()) {
        if (id == dbmenupackagelist->get(idp, "f_package").toInt()) {
            lst.append(idp);
        }
    }
    return lst;
}
