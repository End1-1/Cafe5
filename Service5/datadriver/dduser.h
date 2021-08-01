#ifndef DDUSER_H
#define DDUSER_H

#include "database.h"

class DDUser
{
public:
    DDUser();

    bool authUserPass(Database &db, const QString &email, const QString &phone, const QString &pass);

    QString fSession;

    int fId;
};

#endif // DDUSER_H
