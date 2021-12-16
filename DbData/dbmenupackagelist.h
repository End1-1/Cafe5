#ifndef DBMENUPACKAGELIST_H
#define DBMENUPACKAGELIST_H

#include "dbdata.h"

class DbMenuPackageList : public DbData
{
public:
    DbMenuPackageList();

    DbMenuPackageList(int id);

    int package();

    int dish();

    double qty();

    double price();

    int store();

    QString print1();

    QString print2();

    QList<int> listOf(int id);
};

#endif // DBMENUPACKAGELIST_H
