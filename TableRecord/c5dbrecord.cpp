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

TableRecord &TableRecord::queryRecordOfQuery(C5Database &db, const QMap<QString, QVariant> &condition)
{
    throw std::exception("queryRecord(C5Database &db, const QMap<QString, QVariant> &condition) not implemented");
}

bool TableRecord::getWriteResult(C5Database &db, bool result, QString &err)
{
    if (!result) {
        err = db.fLastError;
    }
    return result;
}
