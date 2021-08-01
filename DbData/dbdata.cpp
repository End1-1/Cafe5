#include "dbdata.h"

QStringList DbData::fDbParams;

DbData::DbData(const QString &tableName, const QString &cond) :
    QObject(),
    fDb(fDbParams)
{
    fTable = tableName;
    fCondition = cond;
    getFromDatabase();
}

QVariant DbData::get(int id, const QString &key)
{
    return fData[id][key];
}

void DbData::setDBParams(const QStringList &dbParams)
{
    fDbParams = dbParams;
}

QString DbData::name(int id)
{
    return get(id, "f_name").toString();
}

QList<int> DbData::list()
{
    return fData.keys();
}

void DbData::refresh()
{
    getFromDatabase();
}

void DbData::getFromDatabase()
{
    bool success = fDb.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    Q_ASSERT(success);
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fData[fDb.getInt("f_id")] = row;
    }
}
