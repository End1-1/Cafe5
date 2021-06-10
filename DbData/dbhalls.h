#ifndef DBHALLS_H
#define DBHALLS_H

#include "dbdata.h"

class DbHalls : public DbData
{
public:
    DbHalls();

    int settings(int id);
};

#endif // DBHALLS_H
