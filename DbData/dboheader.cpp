#include "dboheader.h"
#include "c5database.h"

DbOHeader::DbOHeader() :
    DbData("o_header", "f_state=1 or f_state=5 or f_state=6")
{

}

QDate DbOHeader::dateOpen(const QString &id)
{
    return get(id, "f_dateopen").toDate();
}

QTime DbOHeader::timeOpen(const QString &id)
{
    return get(id, "f_timeopen").toTime();
}

int DbOHeader::staff(const QString &id)
{
    return get(id, "f_staff").toInt();
}

int DbOHeader::state(const QString &id)
{
    return get(id, "f_state").toInt();
}

double DbOHeader::amount(const QString &id)
{
    return get(id, "f_amounttotal").toDouble();
}

int DbOHeader::precheck(const QString &id)
{
    return get(id, "f_precheck").toInt();
}

int DbOHeader::print(const QString &id)
{
    return get(id, "f_print").toInt();
}

QString DbOHeader::comment(const QString &id)
{
    return get(id, "f_comment").toString();
}

void DbOHeader::getFromDatabase()
{
    C5Database db(fDbParams);
    db.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    fOrderData.clear();
    fTableOrder.clear();
    fStaffTable.clear();
    while (db.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < db.columnCount(); i++) {
            row[db.columnName(i)] = db.getValue(i);
        }
        fTableOrder[db.getInt("f_table")] = db.getString("f_id");
        fStaffTable[db.getInt("f_staff")] = db.getInt("f_table");
        fOrderData[db.getString("f_id")] = row;
    }
}

QVariant DbOHeader::get(const QString &id, const QString &key)
{
    return fOrderData[id][key];
}
