#ifndef C5CACHEUSERSGROUPS_H
#define C5CACHEUSERSGROUPS_H

#include "c5cache.h"

#define cache_users_groups 1

class C5CacheUsersGroups : public C5Cache
{
public:
    C5CacheUsersGroups(const QString &host, const QString &db, const QString &user, const QString &password);

    virtual void loadFromDatabase();
};

#endif // C5CACHEUSERSGROUPS_H
