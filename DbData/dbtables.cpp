#include "dbtables.h"
#include "c5utils.h"

DbTables::DbTables() :
    DbData("h_tables")
{
}

int DbTables::hall(int id)
{
    return get(id, "f_hall").toInt();
}

int DbTables::specialConfig(int id)
{
    return get(id, "f_special_config").toInt();
}

bool DbTables::openTable(int table, QStringList &orders, QString &err)
{
    fDb.startTransaction();
    fDb[":f_id"] = table;
    fDb.exec("select * from h_tables where f_id=:f_id for update");
    if (!fDb.nextRow()) {
        err = fDb.fLastError;
        fDb.commit();
        return false;
    }
    QDateTime lockTime = fDb.getDateTime("f_locktime");
    if (lockTime.isValid()) {
        if (lockTime.msecsTo(QDateTime::currentDateTime()) < 60000) {
            if (fDb.getString("f_locksrc") != hostinfo) {
                err = QObject::tr("Table already locked");
                fDb.commit();
                return false;
            }
        }
    }
    fDb[":f_locktime"] = QDateTime::currentDateTime();
    fDb[":f_lockSrc"] = hostinfo;
    fDb.update("h_tables", "f_id", table);
    fDb.commit();

    fDb[":f_state"] = ORDER_STATE_OPEN;
    fDb[":f_table"] = table;
    fDb.exec("select o.f_id from o_header o where o.f_table=:f_table and o.f_state=:f_state");
    while (fDb.nextRow()) {
        orders.append(fDb.getString("f_id"));
    }
    return true;
}

bool DbTables::closeTable(int id, QString &err)
{
    fDb[":f_lock"] = 0;
    fDb[":f_lockSrc"] = "";
    fDb[":f_locktime"] = QVariant();
    fDb[":f_id"] = id;
    if (!fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc, f_locktime=:f_locktime where f_id=:f_id")) {
        err = fDb.fLastError;
        return false;
    }
    return true;
}
