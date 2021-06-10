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
}
