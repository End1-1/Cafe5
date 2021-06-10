#include "shopmanager.h"

ShopManager *ShopManager::fInstance = nullptr;

ShopManager::ShopManager()
{

}

void ShopManager::init(const QString &databaseName)
{
    fInstance = new ShopManager();
    fInstance->fDatabaseName = databaseName;
}

QString ShopManager::databaseName()
{
    return fInstance->fDatabaseName;
}
