#include "tablerecord.h"

TableRecord::TableRecord(const QString &table, Database &db) :
    fTableName(table),
    fDb(db)
{
    if (fValues.contains(fTableName) == false) {
        db.exec(QString("show columns from %1").arg(fTableName));
        while (db.next()) {
            fValues[fTableName][":" + db.string("Field").toLower()] = QVariant();
        }
    }
}

QVariant &TableRecord::operator [](const QString &name)
{
    return fValues[fTableName][name.toLower()];
}

bool TableRecord::insert()
{
    bindValues();
    bool v = fDb.insert(fTableName);
    clearValues();
    return v;
}

bool TableRecord::insert(int &id)
{
    bindValues();
    bool v = fDb.insert(fTableName, id);
    clearValues();
    return v;
}

bool TableRecord::update(const QString &fieldName, const QVariant &fieldValue)
{
    bindValues();
    bool v = fDb.update(fTableName, fieldName, fieldValue);
    clearValues();
    return v;
}

void TableRecord::bindValues()
{
    for (QHash<QString, QVariant>::const_iterator it = fValues[fTableName].constBegin(); it != fValues[fTableName].constEnd(); it++) {
#ifdef QT_DEBUG
        Q_ASSERT(it.value().isValid());
#endif
      fDb[it.key()] = it.value();
    }
}

void TableRecord::clearValues()
{
    for (QHash<QString, QVariant>::iterator it = fValues[fTableName].begin(); it != fValues[fTableName].end(); it++) {
      it.value() = QVariant();
    }
}
