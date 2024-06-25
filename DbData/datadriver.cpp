#include "datadriver.h"
#include "dlgsplashscreen.h"

DataDriver *DataDriver::fInstance = nullptr;
QStringList DataDriver::norefresh;

DataDriver::DataDriver()
{
}

void DataDriver::init(const QStringList &dbParams, DlgSplashScreen *info)
{
    if (!fInstance) {
        fInstance = new DataDriver();
    }
    DbData::setDBParams(dbParams);
    fInstance->fDbUser = new DbUsers();
    info->messageSignal("users...");
    fInstance->fDbTable = new DbTables();
    fInstance->fDbHalls = new DbHalls();
    fInstance->fDbDishes = new DbDishes();
    fInstance->fDbStore = new DbStore();
    fInstance->fDbCar = new DbCar();
    info->messageSignal("partners...");
    fInstance->fDbPartner = new DbPartner();
    fInstance->fDbDishPart1 = new DbDishPart1();
    info->messageSignal("goods...");
    if (!norefresh.contains("goods")) {
        fInstance->fDbGoodsGroup = new DbGoodsGroup();
        fInstance->fDbGoods = new DbGoods();
    }
    fInstance->fDbMenuName = new DbMenuName();
    fInstance->fDbDishPart2 = new DbDishPart2();
    fInstance->fDbMenu = new DbMenu();
    fInstance->fDbDishSpecial = new DbDishSpecial();
    fInstance->fDbUnit = new DbUnit();
    fInstance->fDbBodyState = new DbBodyState();
    fInstance->fDbMenuPackage = new DbMenuPackages();
    fInstance->fDbMenuPackageList = new DbMenuPackageList();
    fInstance->fDbStoreDocType = new DbStoreDocType();
    fInstance->fDbDishComments = new DbDishComments();
    fInstance->fDbDishRemoveReason = new DbDishRemoveReason();
    fInstance->fDbCurrency = new DbCurrency();
    info->messageSignal("datadriver complete.");
}
