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

    fInstance->fDbStore = new DbStore();
    emit(info->messageSignal("partners..."));
    emit(info->messageSignal("goods..."));

    if(!norefresh.contains("goods")) {
        fInstance->fDbGoodsGroup = new DbGoodsGroup();
        fInstance->fDbGoods = new DbGoods();
    }

    fInstance->fDbUnit = new DbUnit();
    fInstance->fDbStoreDocType = new DbStoreDocType();
    info->messageSignal("datadriver complete.");
}
