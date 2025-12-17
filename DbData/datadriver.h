#ifndef DATADRIVER_H
#define DATADRIVER_H

#include "dbstore.h"
#include "dbgoodsgroup.h"
#include "dbunit.h"
#include "dbgoods.h"
#include "dbstoredoctype.h"
#include <QStringList>

#define dbstore DataDriver::fInstance->fDbStore
#define dbstoredoctype DataDriver::fInstance->fDbStoreDocType
#define dbshift DataDriver::fInstance->fDbShift
#define dbgoods DataDriver::fInstance->fDbGoods
#define dbgoodsgroup DataDriver::fInstance->fDbGoodsGroup
#define dbunit DataDriver::fInstance->fDbUnit

class DlgSplashScreen;

class DataDriver
{
public:
    DataDriver();

    DbStore* fDbStore;

    DbStoreDocType* fDbStoreDocType;

    DbGoodsGroup* fDbGoodsGroup;

    DbGoods* fDbGoods;

    DbUnit* fDbUnit;

    static void init(const QStringList &dbParams, DlgSplashScreen *info);

    static DataDriver* fInstance;

    static QStringList norefresh;
};

#endif // DATADRIVER_H
