#ifndef SQLQUERIES_H
#define SQLQUERIES_H

#include "database.h"
#include <QHash>

#define sqlquery(x) SqlQueries::get(x)

class SqlQueries
{
public:
    SqlQueries(Database &db);
    static void init(Database &db);
    static QString get(const QString &name);

private:
    QHash<QString, QString> fQueries;
    static SqlQueries *fInstance;
};

#endif // SQLQUERIES_H
