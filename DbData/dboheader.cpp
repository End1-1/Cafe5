#include "dboheader.h"


DbOHeader::DbOHeader() :
    DbData("o_header", "f_state=1")
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

void DbOHeader::getFromDatabase()
{
    fDb.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    fOrderData.clear();
    fTableOrder.clear();
    fStaffTable.clear();
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fTableOrder[fDb.getInt("f_table")] = fDb.getString("f_id");
        fStaffTable[fDb.getInt("f_staff")] = fDb.getInt("f_table");
        fOrderData[fDb.getString("f_id")] = row;
    }
}

QVariant DbOHeader::get(const QString &id, const QString &key)
{
    return fOrderData[id][key];
}
