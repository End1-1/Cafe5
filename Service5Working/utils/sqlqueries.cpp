#include "sqlqueries.h"
#include <QDebug>

SqlQueries *SqlQueries::fInstance = nullptr;

SqlQueries::SqlQueries(Database &db)
{
    db.exec("select fkey, fquery from server5.queries");
    while (db.next()) {
        fQueries[db.string("fkey")] = db.string("fquery");
    }
}

void SqlQueries::init(Database &db)
{
    if (fInstance == nullptr) {
        fInstance = new SqlQueries(db);
    }
}

QString SqlQueries::get(const QString &name)
{
    qDebug() << fInstance << "Instance of sql queries";
    Q_ASSERT(fInstance->fQueries.contains(name));
    return fInstance->fQueries[name];
}
