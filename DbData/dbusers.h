#ifndef DBUSERS_H
#define DBUSERS_H

#include "dbdata.h"

class DbUsers : public DbData
{
public:
    DbUsers();

    QString fullName(int id);

    QString fullShort(int id);

};

#endif // DBUSERS_H
