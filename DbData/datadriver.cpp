#include "datadriver.h"
#include "dlgsplashscreen.h"
#include "c5database.h"
#include "working.h"

DataDriver* DataDriver::fInstance = nullptr;
QStringList DataDriver::norefresh;

DataDriver::DataDriver()
{
}

void DataDriver::init(const QStringList &dbParams, DlgSplashScreen *info)
{
    C5Database db;

    if(!fInstance) {
        fInstance = new DataDriver();
    }

    fInstance->fDbUser = new DbUsers();
    emit(info->messageSignal("users..."));
    fInstance->fDbTable = new DbTables();
    fInstance->fDbHalls = new DbHalls();
    fInstance->fDbDishes = new DbDishes();
    fInstance->fDbStore = new DbStore();
    fInstance->fDbCar = new DbCar();
    emit(info->messageSignal("partners..."));
    fInstance->fDbPartner = new DbPartner();
    fInstance->fDbDishPart1 = new DbDishPart1();
    emit(info->messageSignal("goods..."));

    if(!norefresh.contains("goods")) {
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
    emit(info->messageSignal("flags..."));
    db.exec("select * from o_flags");

    while(db.nextRow()) {
        Working::fFlags[db.getInt("f_id")] = {db.getInt("f_id"), db.getInt("f_enabled"), db.getString("f_field"), db.getString("f_name")};
    }

    info->messageSignal("datadriver complete.");
}
