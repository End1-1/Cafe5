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
#include "dbunit.h"
#include "dbgoods.h"
#include "dbmenuname.h"
#include "dbdishpart2.h"
#include "dbshift.h"
#include "dbmenu.h"
#include "dbdishspecial.h"
#include "dbbodystate.h"
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
#define dbmenuname DataDriver::fInstance->fDbMenuName
#define dbdishpart2 DataDriver::fInstance->fDbDishPart2
#define dbmenu DataDriver::fInstance->fDbMenu
#define dbdishspecial DataDriver::fInstance->fDbDishSpecial
#define dbshift DataDriver::fInstance->fDbShift
#define dbgoods DataDriver::fInstance->fDbGoods
#define dbgoodsgroup DataDriver::fInstance->fDbGoodsGroup
#define dbunit DataDriver::fInstance->fDbUnit
#define dbbodystate DataDriver::fInstance->fDbBodyState

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

    DbUnit *fDbUnit;

    DbMenuName *fDbMenuName;

    DbDishPart2 *fDbDishPart2;

    DbMenu *fDbMenu;

    DbDishSpecial *fDbDishSpecial;

    DbShift *fDbShift;

    DbBodyState *fDbBodyState;

    static void init(const QStringList &dbParams);

    static DataDriver *fInstance;
};

#endif // DATADRIVER_H
