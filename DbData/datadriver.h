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
#include "dbgoodsgroup.h"
#include "dbunit.h"
#include "dbgoods.h"
#include "dbmenuname.h"
#include "dbdishpart2.h"
#include "dbmenu.h"
#include "dbdishspecial.h"
#include "dbbodystate.h"
#include "dbmenupackages.h"
#include "dbmenupackagelist.h"
#include "dbstoredoctype.h"
#include "dbdishcomments.h"
#include "dbdishremovereason.h"
#include "dbcurrency.h"
#include <QStringList>

#define dbuser DataDriver::fInstance->fDbUser
#define dbtable DataDriver::fInstance->fDbTable
#define dbhall DataDriver::fInstance->fDbHalls
#define dbdish DataDriver::fInstance->fDbDishes
#define dbstore DataDriver::fInstance->fDbStore
#define dbcar DataDriver::fInstance->fDbCar
#define dbpartner DataDriver::fInstance->fDbPartner
#define dbdishpart1 DataDriver::fInstance->fDbDishPart1
#define dbmenuname DataDriver::fInstance->fDbMenuName
#define dbdishpart2 DataDriver::fInstance->fDbDishPart2
#define dbmenu DataDriver::fInstance->fDbMenu
#define dbdishspecial DataDriver::fInstance->fDbDishSpecial
#define dbstoredoctype DataDriver::fInstance->fDbStoreDocType
#define dbshift DataDriver::fInstance->fDbShift
#define dbgoods DataDriver::fInstance->fDbGoods
#define dbgoodsgroup DataDriver::fInstance->fDbGoodsGroup
#define dbunit DataDriver::fInstance->fDbUnit
#define dbbodystate DataDriver::fInstance->fDbBodyState
#define dbmenupackage DataDriver::fInstance->fDbMenuPackage
#define dbmenupackagelist DataDriver::fInstance->fDbMenuPackageList
#define dbdishcomment DataDriver::fInstance->fDbDishComments
#define dbdishremovereason DataDriver::fInstance->fDbDishRemoveReason
#define dbcurrency DataDriver::fInstance->fDbCurrency

class DlgSplashScreen;

class DataDriver
{
public:
    DataDriver();

    DbUsers *fDbUser;

    DbTables *fDbTable;

    DbHalls *fDbHalls;

    DbDishes *fDbDishes;

    DbStore *fDbStore;

    DbStoreDocType *fDbStoreDocType;

    DbCar *fDbCar;

    DbPartner *fDbPartner;

    DbDishPart1 *fDbDishPart1;

    DbGoodsGroup *fDbGoodsGroup;

    DbGoods *fDbGoods;

    DbUnit *fDbUnit;

    DbMenuName *fDbMenuName;

    DbDishPart2 *fDbDishPart2;

    DbMenu *fDbMenu;

    DbDishSpecial *fDbDishSpecial;

    DbBodyState *fDbBodyState;

    DbMenuPackages *fDbMenuPackage;

    DbMenuPackageList *fDbMenuPackageList;

    DbDishComments *fDbDishComments;

    DbDishRemoveReason *fDbDishRemoveReason;

    DbCurrency *fDbCurrency;

    static void init(const QStringList &dbParams, DlgSplashScreen *info);

    static DataDriver *fInstance;

    static QStringList norefresh;
};

#endif // DATADRIVER_H
