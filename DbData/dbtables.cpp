#include "dbtables.h"
#include "c5utils.h"
#include "c5database.h"

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
    C5Database db(fDbParams);
    db.startTransaction();
    db[":f_id"] = table;
    db.exec("select * from h_tables where f_id=:f_id for update");
    if (!db.nextRow()) {
        err = db.fLastError;
        db.commit();
        return false;
    }
    QDateTime lockTime = db.getDateTime("f_locktime");
    if (lockTime.isValid()) {
        if (lockTime.msecsTo(QDateTime::currentDateTime()) < 60000) {
            if (db.getString("f_locksrc") != hostinfo) {
                err = QObject::tr("Table already locked");
                db.commit();
                return false;
            }
        }
    }
    db[":f_locktime"] = QDateTime::currentDateTime();
    db[":f_lockSrc"] = hostinfo;
    db.update("h_tables", "f_id", table);
    db.commit();

    db[":f_state"] = ORDER_STATE_OPEN;
    db[":f_table"] = table;
    db.exec("select o.f_id from o_header o where o.f_table=:f_table and o.f_state=:f_state");
    while (db.nextRow()) {
        orders.append(db.getString("f_id"));
    }
    return true;
}

bool DbTables::closeTable(int id, QString &err)
{
    C5Database db(fDbParams);
    db[":f_lock"] = 0;
    db[":f_lockSrc"] = "";
    db[":f_locktime"] = QVariant();
    db[":f_id"] = id;
    if (!db.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc, f_locktime=:f_locktime where f_id=:f_id")) {
        err = db.fLastError;
        return false;
    }
    return true;
}