#ifndef DBDISHSPECIAL_H
#define DBDISHSPECIAL_H

#include "dbdata.h"

class DbDishSpecial : public DbData
{
public:
    DbDishSpecial();

    bool contains(int dishid);

    virtual void refresh() override;

    QStringList special(int dishid);

private:
    QMap<int, QStringList> fSpecial;
};

#endif // DBDISHSPECIAL_H
