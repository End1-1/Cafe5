#include "shopmanager.h"
#include "logwriter.h"

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
    if (!fInstance) {
        LogWriter::write(LogWriterLevel::warning, "", "ShopManager requested, but not initialized");
        return "SHOPMANAGER NOT INITIALIZED";
    }
    return fInstance->fDatabaseName;
}
