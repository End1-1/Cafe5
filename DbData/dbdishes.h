#ifndef DBDISHES_H
#define DBDISHES_H

#include "dbdata.h"

class DbDishes : public DbData
{
public:
    DbDishes();

    bool isHourlyPayment(int id);

    int part1(int id);

    int part2(int id);

    QString part1name(int id);

    QString part2name(int id);

    int color(int id);

    bool canService(int id);

    bool canDiscount(int id);

    int group(int id);
};

#endif // DBDISHES_H
