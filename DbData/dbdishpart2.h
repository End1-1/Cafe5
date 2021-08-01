#ifndef DBDISHPART2_H
#define DBDISHPART2_H

#include "dbdata.h"

class DbDishPart2 : public DbData
{
public:
    DbDishPart2();

    int part1(int id);

    int parentid(int id);

    QString adgcode(int id);
};

#endif // DBDISHPART2_H
