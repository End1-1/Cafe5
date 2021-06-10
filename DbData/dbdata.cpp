#include "dbdata.h"

QStringList DbData::fDbParams;

DbData::DbData(const QString &tableName) :
    fDb(fDbParams)
{
    getFromDatabase(tableName);
}

QVariant DbData::get(int id, const QString &key)
{
    return fData[id][key];
}

void DbData::setDBParams(const QStringList &dbParams)
{

}

void DbData::getFromDatabase(const QString &tableName)
{
    fDb.exec("select * from " + tableName);
    fData.clear();
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fData[fDb.getInt("f_id")] = row;
    }
}
