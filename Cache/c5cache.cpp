#include "c5cache.h"

QMap<QString, C5Cache*> C5Cache::fCacheList;
QMap<int, QString> C5Cache::fCacheQuery;

C5Cache::C5Cache(const QStringList &dbParams) :
    fDb(dbParams)
{
    if (fCacheQuery.count() == 0) {
        fCacheQuery[cache_users_groups] = "select f_id, f_name from s_user_group order by f_name";
        fCacheQuery[cache_users_states] = "select f_id, f_name from s_user_state";
        fCacheQuery[cache_dish_part1] = "select f_id, f_name from d_part1";
    }
}

C5Cache *C5Cache::cache(const QStringList &dbParams, int cacheId)
{
    QString cacheName = dbParams[0] + dbParams[1] + dbParams[2] + dbParams[3] + "-" + QString::number(cacheId);
    C5Cache *cache = 0;
    if (!fCacheList.contains(cacheName)) {
        cache = new C5Cache(dbParams);
        QString query = fCacheQuery[cacheId];
        cache->loadFromDatabase(query);
        fCacheList[cacheName] = cache;
    }

    return fCacheList[cacheName];
}

void C5Cache::loadFromDatabase(const QString &query)
{
    fDb.exec(query, fCacheData);
}
