#include "dbopreorder.h"
#include "c5database.h"

DbOPreorder::DbOPreorder() :
    DbData("o_preorder", " f_id in (select f_id from o_header where f_state=5 or f_state=6)")
{

}

QString DbOPreorder::guestName(const QString &id)
{
    return get(id, "f_guestname").toString();
}

double DbOPreorder::amount(const QString &id)
{
    return get(id, "f_prepaidcash").toDouble()
            + get(id, "f_prepaidcard").toDouble()
            + get(id, "f_prepaidpayx").toDouble();
}

int DbOPreorder::table(const QString &id)
{
    return get(id, "f_fortable").toInt();
}

void DbOPreorder::getFromDatabase()
{
    C5Database db(fDbParams);
    db.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    fOrderData.clear();
    while (db.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < db.columnCount(); i++) {
            row[db.columnName(i)] = db.getValue(i);
        }
        fOrderData[db.getString("f_id")] = row;
    }
}

QVariant DbOPreorder::get(const QString &id, const QString &key)
{
    return fOrderData[id][key];
}
