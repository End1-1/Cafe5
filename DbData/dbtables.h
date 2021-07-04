#ifndef DBTABLES_H
#define DBTABLES_H

#include "dbdata.h"

class DbTables : public DbData
{
    Q_OBJECT

public:
    DbTables();

    int hall(int id);

    int specialConfig(int id);

    bool openTable(int table, QStringList &orders, QString &err);

    bool closeTable(int id, QString &err);
};

#endif // DBTABLES_H
