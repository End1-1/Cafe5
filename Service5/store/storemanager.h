#ifndef STOREMANAGER_H
#define STOREMANAGER_H

#include <QString>

class Store;

class StoreManager
{
public:
    StoreManager();
    static void init(const QString &databaseName);

private:
    QString fDatabaseName;
    static StoreManager *fInstance;
    static QMap<int, Store*> fStores;
    static Store *getStore(int id);
};

#endif // STOREMANAGER_H
