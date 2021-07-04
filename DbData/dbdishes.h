#ifndef DBDISHES_H
#define DBDISHES_H

#include "dbdata.h"

class DbDishes : public DbData
{
public:
    DbDishes();

    bool isHourlyPayment(int id);
};

#endif // DBDISHES_H
