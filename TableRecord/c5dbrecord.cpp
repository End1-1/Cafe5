#include "c5dbrecord.h"
#include <QException>

bool TableRecord::getRecord(C5Database &db)
{
    throw std::exception("getRecord() not implemented");
}

TableRecord &TableRecord::queryRecordOfId(C5Database &db, const QVariant &id)
{
    throw std::exception("queryRecord(C5Database &db, const QVariant &id) not implemented");
}

bool TableRecord::getWriteResult(C5Database &db, bool result, QString &err)
{
    if (!result) {
        err = db.fLastError;
    }
    return result;
}

bool TableRecord::insert(C5Database &db, const QString &table, QString &err)
{
    bind(db);
    return getWriteResult(db, db.insert(table, false), err);
}

bool TableRecord::insertWithId(C5Database &db, const QString &table, QString &err)
{
    bind(db);
    int newId;
    bool r = getWriteResult(db, db.insert(table, newId), err);
    if (r) {
        id = newId;
    }
    return newId > 0;
}

bool TableRecord::update(C5Database &db, const QString &table, QString &err)
{
    bind(db);
    return getWriteResult(db, db.update(table, "f_id", id), err);
}
