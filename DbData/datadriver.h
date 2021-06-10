#ifndef DATADRIVER_H
#define DATADRIVER_H

#include "dbusers.h"
#include "dbtables.h"
#include "dbhalls.h"
#include <QStringList>

#define dbuser DataDriver::fInstance->fDbUser
#define dbtable DataDriver::fInstance->fDbTable
#define dbhall DataDriver::fInstance->fDbHalls

class DataDriver
{
public:
    DataDriver();

    DbUsers *fDbUser;

    DbTables *fDbTable;

    DbHalls *fDbHalls;

    static void init(const QStringList &dbParams);

    static DataDriver *fInstance;
};

#endif // DATADRIVER_H
