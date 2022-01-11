#ifndef DBHALLS_H
#define DBHALLS_H

#include "dbdata.h"

class DbHalls : public DbData
{
public:
    DbHalls();

    DbHalls(int id);

    int settings(int id);

    int booking(int id);


};

#endif // DBHALLS_H
