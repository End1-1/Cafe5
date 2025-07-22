#include "dataonline.h"
#include "c5database.h"
#include "c5config.h"

QHash<QString, DataOnline*> DataOnline::fInstances;

DataOnline::DataOnline() :
    QObject(nullptr)
{
    C5Database db;

    if(fColumnNames.isEmpty()) {
        db.exec("select * from sys_cache_tables");

        while(db.nextRow()) {
            fColumnNames.insert(db.getString("f_name"), QHash<QString, int>());
            fTableQueries[db.getString("f_name")] = db.getString("f_sql");
        }

        for(const QString &table : fColumnNames.keys()) {
            db.exec(fTableQueries[table] + " where t.f_id=-1");

            for(QHash<QString, int>::const_iterator it = db.fNameColumnMap.constBegin(); it != db.fNameColumnMap.constEnd();
                    it++) {
                fColumnNames[table][it.key().toLower()] = it.value();
            }
        }
    }
}

QVariant DataOnline::value(const QString &table, const QString &field, int id)
{
    Q_ASSERT(fTableQueries.contains(table));
    bool needupdate = false;

    if(!fIdRow.contains(table)) {
        needupdate = true;
    }

    if(!needupdate) {
        if(!fIdRow[table].contains(id)) {
            needupdate = true;
        }
    }

    if(needupdate) {
        C5Database db;
        db[":f_id"] = id;
        std::vector<QJsonArray > values;
        QString sql = fTableQueries[table] + " where t.f_id=:f_id";
        db.exec(sql, values);
        QVector<QJsonArray >& data = fValues[table];

        if(values.size() == 0) {
            return "INVALID ID";
        }

        data.append(values.at(0));
        fIdRow[table][id] = data.count() - 1;
    }

    Q_ASSERT(fColumnNames[table].contains(field.toLower()));
    return fValues[table].at(fIdRow[table][id]).at(fColumnNames[table][field.toLower()]);
}

DataOnline* DataOnline::instance()
{
    QString hash = __c5config.dbParams().join(',');

    if(fInstances.contains(hash)) {
        return fInstances[hash];
    } else {
        fInstances[hash] = new DataOnline();
    }

    return fInstances[hash];
}
