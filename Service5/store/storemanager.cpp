#include "storemanager.h"

StoreManager *StoreManager::fInstance = nullptr;

StoreManager::StoreManager()
{

}

void StoreManager::init(const QString &databaseName)
{
    fInstance = new StoreManager();
    fInstance->fDatabaseName = databaseName;
}
