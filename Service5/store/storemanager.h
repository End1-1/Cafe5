#ifndef STOREMANAGER_H
#define STOREMANAGER_H

#include "storerecord.h"
#include <QString>

class Store;

class StoreManager
{
public:
    StoreManager();
    static void init(const QString &databaseName);
    static int queryQty(int store, const QStringList &sku, QList<StoreRecord> &sr);

private:
    QString fDatabaseName;
    static StoreManager *fInstance;
    static QMap<int, Store*> fStores;
    static Store *getStore(int id);
};

#endif // STOREMANAGER_H
