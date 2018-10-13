#include "c5cache.h"
#include "c5cacheusersgroups.h"
#include "c5cacheusersstate.h"

QMap<QString, C5Cache*> C5Cache::fCacheList;

C5Cache::C5Cache(const QString &host, const QString &db, const QString &user, const QString &password)
{
    fDb.setDatabase(host, db, user, password);
}

C5Cache *C5Cache::cache(const QString &host, const QString &db, const QString &user, const QString &password, int cacheId)
{
    QString cacheName = host + db + user + password + "-" + QString::number(cacheId);
    C5Cache *cache = 0;
    if (!fCacheList.contains(cacheName)) {
        switch (cacheId) {
        case cache_users_groups:
            cache = new C5CacheUsersGroups(host, db, user, password);
            break;
        case cache_users_states:
            cache = new C5CacheUsersState(host, db, user, password);
            break;
        }
        cache->loadFromDatabase();
        fCacheList[cacheName] = cache;
    }

    return fCacheList[cacheName];
}
