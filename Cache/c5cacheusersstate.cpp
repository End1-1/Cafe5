#include "c5cacheusersstate.h"

C5CacheUsersState::C5CacheUsersState(const QString &host, const QString &db, const QString &user, const QString &password) :
    C5Cache(host, db, user, password)
{

}

void C5CacheUsersState::loadFromDatabase()
{
    fDb.exec("select f_id, f_name from s_user_state", fCacheData);
}
