#ifndef DBUNIT_H
#define DBUNIT_H

#include "dbdata.h"

class DbUnit : public DbData
{
public:
    DbUnit();

    DbUnit(int id);

    QString unitName();

    double defaultQty();
};

#endif // DBUNIT_H
