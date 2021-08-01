#include "dbdishspecial.h"

DbDishSpecial::DbDishSpecial() :
    DbData("d_special")
{

}

bool DbDishSpecial::contains(int dishid)
{
    return fSpecial.contains(dishid);
}

void DbDishSpecial::refresh()
{
    DbData::refresh();
    fSpecial.clear();
    for (QMap<int, QMap<QString, QVariant> >::const_iterator it = fData.begin(); it != fData.end(); it++) {
        fSpecial[it.value()["f_dish"].toInt()].append(it.value()["f_comment"].toString());
    }
}

QStringList DbDishSpecial::special(int dishid)
{
    return fSpecial[dishid];
}
