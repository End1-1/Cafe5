#ifndef STOREMANAGER_H
#define STOREMANAGER_H

#include <QString>

class StoreManager
{
public:
    StoreManager();
    static void init(const QString &databaseName);

private:
    QString fDatabaseName;
    static StoreManager *fInstance;
};

#endif // STOREMANAGER_H
