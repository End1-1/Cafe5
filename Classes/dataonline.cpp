#include "dataonline.h"
#include "c5database.h"

QHash<QString, DataOnline*> DataOnline::fInstances;

DataOnline::DataOnline(const QStringList &dbParams) :
    QObject(nullptr),
    fDBParams(dbParams)
{
    C5Database db(dbParams);
    if (fColumnNames.isEmpty()) {
        db.exec("select * from sys_cache_tables");
        while (db.nextRow()) {
            fColumnNames.insert(db.getString("f_name"), QHash<QString, int>());
            fTableQueries[db.getString("f_name")] = db.getString("f_sql");
        }
        for (const QString &table: fColumnNames.keys()) {
            db.exec(fTableQueries[table] + " where t.f_id=-1");
            fColumnNames[table] = db.fNameColumnMap;
        }
    }
}

QVariant DataOnline::value(const QString &table, const QString &field, int id)
{
    Q_ASSERT(fTableQueries.contains(table));
    bool needupdate = false;
    if (!fIdRow.contains(table)) {
        needupdate = true;
    }
    if (!needupdate) {
        if (!fIdRow[table].contains(id)) {
            needupdate = true;
        }
    }
    if (needupdate) {
        C5Database db(fDBParams);
        db[":f_id"] = id;
        QList<QList<QVariant> > values;
        db.exec(fTableQueries[table] + " where t.f_id=:f_id", values);
        QList<QList<QVariant> > &data = fValues[table];
        if (values.count() == 0) {
            return "INVALID ID";
        }
        data.append(values.at(0));
        fIdRow[table][id] = data.count() - 1;
    }
    Q_ASSERT(fColumnNames[table].contains(field.toLower()));
    return fValues[table].at(fIdRow[table][id]).at(fColumnNames[table][field.toLower()]);
}

DataOnline *DataOnline::instance(const QStringList &dbParams)
{
    QString hash = dbParams.join(',');
    if (fInstances.contains(hash)) {
        return fInstances[hash];
    } else {
        fInstances[hash] = new DataOnline(dbParams);
    }
    return fInstances[hash];
}
