#ifndef DBCURRENCY_H
#define DBCURRENCY_H

#include "dbdata.h"

class DbCurrency : public DbData
{
public:
    DbCurrency();

    DbCurrency(int id);

    QString currencyName();
};

#endif // DBCURRENCY_H
