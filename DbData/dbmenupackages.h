#ifndef DBMENUPACKAGES_H
#define DBMENUPACKAGES_H

#include "dbdata.h"

class DbMenuPackages : public DbData
{
public:
    DbMenuPackages();

    DbMenuPackages(int id);

    QString name();

    double price();

    bool isEnabled();
};

#endif // DBMENUPACKAGES_H
