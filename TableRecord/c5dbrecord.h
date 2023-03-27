#ifndef C5DBRECORD_H
#define C5DBRECORD_H

#include "c5database.h"
#include <QObject>

class TableRecord {
public:
    virtual bool write(C5Database &db, QString &err) = 0;
    virtual bool getRecord(C5Database &db);
    virtual TableRecord &queryRecordOfId(C5Database &db, const QVariant &id);
    virtual TableRecord &queryRecordOfQuery(C5Database &db, const QMap<QString, QVariant> &condition);
    bool getWriteResult(C5Database &db, bool result, QString &err);
};


#endif // C5DBRECORD_H
