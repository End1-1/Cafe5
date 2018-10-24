#ifndef C5CACHE_H
#define C5CACHE_H

#include "c5database.h"
#include <QMap>

#define cache_users_groups 1
#define cache_users_states 2
#define cache_dish_part1 3

class C5Cache
{
public:
    C5Cache(const QStringList &dbParams);

    inline QString getString(int row, int column) {return fCacheData.at(row).at(column).toString();}

    inline int getInt(int row, int column) {return fCacheData.at(row).at(column).toInt();}

    inline int rowCount() {return fCacheData.count();}

    static C5Cache *cache(const QStringList &dbParams, int cacheId);

    static QMap<QString, C5Cache*> fCacheList;

protected:
    QList<QList<QVariant> > fCacheData;

    virtual void loadFromDatabase(const QString &query);

    C5Database fDb;

private:
    static QMap<int, QString> fCacheQuery;
};

#endif // C5CACHE_H
