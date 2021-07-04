#ifndef DATADRIVER_H
#define DATADRIVER_H

#include "dbusers.h"
#include "dbtables.h"
#include "dbhalls.h"
#include "dbdishes.h"
#include "dbstore.h"
#include "dbpartner.h"
#include "dbcar.h"
#include "dbdishpart1.h"
#include "dboheader.h"
#include "dbgoodsgroup.h"
#include "dbgoods.h"
#include <QStringList>

#define dbuser DataDriver::fInstance->fDbUser
#define dbtable DataDriver::fInstance->fDbTable
#define dbhall DataDriver::fInstance->fDbHalls
#define dbdish DataDriver::fInstance->fDbDishes
#define dbstore DataDriver::fInstance->fDbStore
#define dbcar DataDriver::fInstance->fDbCar
#define dbpartner DataDriver::fInstance->fDbPartner
#define dbdishpart1 DataDriver::fInstance->fDbDishPart1
#define dboheader DataDriver::fInstance->fDbOHeader

class DataDriver
{
public:
    DataDriver();

    DbUsers *fDbUser;

    DbTables *fDbTable;

    DbHalls *fDbHalls;

    DbDishes *fDbDishes;

    DbStore *fDbStore;

    DbCar *fDbCar;

    DbPartner *fDbPartner;

    DbDishPart1 *fDbDishPart1;

    DbOHeader *fDbOHeader;

    DbGoodsGroup *fDbGoodsGroup;

    DbGoods *fDbGoods;

    static void init(const QStringList &dbParams);

    static DataDriver *fInstance;
};

#endif // DATADRIVER_H
