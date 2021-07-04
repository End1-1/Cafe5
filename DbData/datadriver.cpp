#include "datadriver.h"

DataDriver *DataDriver::fInstance = nullptr;

DataDriver::DataDriver()
{

}

void DataDriver::init(const QStringList &dbParams)
{
    fInstance = new DataDriver();
    DbData::setDBParams(dbParams);
    fInstance->fDbUser = new DbUsers();
    fInstance->fDbTable = new DbTables();
    fInstance->fDbHalls = new DbHalls();
    fInstance->fDbDishes = new DbDishes();
    fInstance->fDbStore = new DbStore();
    fInstance->fDbCar = new DbCar();
    fInstance->fDbPartner = new DbPartner();
    fInstance->fDbDishPart1 = new DbDishPart1();
    fInstance->fDbOHeader = new DbOHeader();
    fInstance->fDbGoodsGroup = new DbGoodsGroup();
    fInstance->fDbGoods = new DbGoods();
}
