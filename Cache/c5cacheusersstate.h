#ifndef C5CACHEUSERSSTATE_H
#define C5CACHEUSERSSTATE_H

#include "c5cache.h"

#define cache_users_states 2

class C5CacheUsersState : public C5Cache
{
public:
    C5CacheUsersState(const QString &host, const QString &db, const QString &user, const QString &password);

    virtual void loadFromDatabase();
};

#endif // C5CACHEUSERSSTATE_H
