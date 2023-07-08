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

    bool bookingOnly(int id);

    bool openTable(int table, QStringList &orders, QString &err);

    bool closeTable(int id, QString &err);

    int hourlyId(int id);
};

#endif // DBTABLES_H
