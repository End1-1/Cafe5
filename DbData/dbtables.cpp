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

int DbTables::hourlyId(int id)
{
    return get(id, "f_hourly").toInt();
}
