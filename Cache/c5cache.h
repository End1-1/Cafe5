#ifndef C5CACHE_H
#define C5CACHE_H

#include "c5database.h"
#include <QMap>

class C5Cache
{
public:
    C5Cache(const QString &host, const QString &db, const QString &user, const QString &password);

    inline QString getString(int row, int column) {return fCacheData.at(row).at(column).toString();}

    inline int getInt(int row, int column) {return fCacheData.at(row).at(column).toInt();}

    inline int rowCount() {return fCacheData.count();}

    static C5Cache *cache(const QString &host, const QString &db, const QString &user, const QString &password, int cacheId);

    static QMap<QString, C5Cache*> fCacheList;

protected:
    QList<QList<QVariant> > fCacheData;

    virtual void loadFromDatabase() = 0;

    C5Database fDb;

private:
};

#endif // C5CACHE_H
