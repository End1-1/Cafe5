#include "c5cacheusersgroups.h"

C5CacheUsersGroups::C5CacheUsersGroups(const QString &host, const QString &db, const QString &user, const QString &password) :
    C5Cache(host, db, user, password)
{

}

void C5CacheUsersGroups::loadFromDatabase()
{
    fDb.exec("select f_id, f_name from s_user_group order by f_name", fCacheData);
}
