#include "shopmanager.h"
#include "logwriter.h"

ShopManager *ShopManager::fInstance = nullptr;

ShopManager::ShopManager()
{

}

void ShopManager::init(const QString &databaseName)
{
    if (fInstance == nullptr) {
        fInstance = new ShopManager();
    }
    fInstance->fDatabaseName = databaseName;
}

void ShopManager::release()
{
    if (fInstance) {
        delete fInstance;
    }
}

QString ShopManager::databaseName()
{
    if (!fInstance) {
        LogWriter::write(LogWriterLevel::warning, "", "ShopManager requested, but not initialized");
        return "SHOPMANAGER NOT INITIALIZED";
    }
    return fInstance->fDatabaseName;
}
