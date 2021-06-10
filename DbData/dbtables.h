#ifndef DBTABLES_H
#define DBTABLES_H

#include "dbdata.h"

class DbTables : public DbData
{
public:
    DbTables();

    QString name(int id);

    int hall(int id);
};

#endif // DBTABLES_H
