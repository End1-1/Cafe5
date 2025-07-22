#include "dbdata.h"
#include "c5database.h"
#include <QDebug>

QStringList DbData::fDbParams;

DbData::DbData(const QString &tableName, const QString &cond) :
    QObject()
{
    Q_ASSERT(!tableName.isEmpty());
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
    if(!fData.contains(id)) {
        C5Database db;
        db[":f_id"] = id;
        db.exec("select * from " + fTable + " where f_id=:f_id");

        if(db.nextRow()) {
            QMap<QString, QVariant> m;
            db.rowToMap(m);
            fData[id] = m;
        }
    }

    return fData[id][key];
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

void DbData::updateField(int id, const QString &field, const QVariant &value)
{
#ifdef QT_DEBUG
    Q_ASSERT(fData.contains(id));
    Q_ASSERT(fData[id].contains(field));
#endif
    fData[id][field] = value;
}

void DbData::getFromDatabase()
{
    fData.clear();

    if(fTable.isEmpty()) {
        return;
    }

    C5Database db;
    bool success = db.exec("select * from " + fTable + (fCondition.isEmpty() ? "" : " where " + fCondition));
    Q_ASSERT(success);

    while(db.nextRow()) {
        QMap<QString, QVariant> row;

        for(int i = 0; i < db.columnCount(); i++) {
            row[db.columnName(i)] = db.getValue(i);
        }

        fData[db.getInt("f_id")] = row;
    }
}
