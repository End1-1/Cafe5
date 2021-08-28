#include "dbdata.h"
#include "c5database.h"
#include <QDebug>

QStringList DbData::fDbParams;

DbData::DbData(const QString &tableName, const QString &cond) :
    QObject()
{
    fTable = tableName;
    fCondition = cond;
    getFromDatabase();
}

DbData::DbData(int id)
{
    fId = id;
}

QVariant DbData::get(int id, const QString &key)
{
#ifdef QT_DEBUG
    Q_ASSERT(fData[id].contains(key));
#endif
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

int DbData::id()
{
    return fId;
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
    if (fTable.isEmpty()) {
        return;
    }
    C5Database db(fDbParams);
    bool success = db.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    Q_ASSERT(success);
    while (db.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < db.columnCount(); i++) {
            row[db.columnName(i)] = db.getValue(i);
        }
        fData[db.getInt("f_id")] = row;
    }
}
