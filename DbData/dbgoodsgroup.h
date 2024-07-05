#ifndef DBGOODSGROUP_H
#define DBGOODSGROUP_H

#include "dbdata.h"

class DbGoodsGroup : public DbData
{
public:
    DbGoodsGroup();

    DbGoodsGroup(int id);

    QString adgt();

    int taxDept();

    QString groupName();
};

#endif // DBGOODSGROUP_H
