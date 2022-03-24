#ifndef SHOPMANAGER_H
#define SHOPMANAGER_H

#include <QString>

class ShopManager
{
public:
    ShopManager();
    static void init(const QString &databaseName);
    static QString databaseName();

private:
    static ShopManager *fInstance;
    QString fDatabaseName;
};

#endif // SHOPMANAGER_H
