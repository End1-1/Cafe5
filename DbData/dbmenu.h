#ifndef DBMENU_H
#define DBMENU_H

#include "dbdata.h"

class DbMenu : public DbData
{
public:
    DbMenu();

    int menuid(int id);

    int dishid(int id);

    bool recent(int id);

    double price(int id);

    int store(int id);

    QString print1(int id);

    QString print2(int id);
};

#endif // DBMENU_H
