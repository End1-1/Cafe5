#ifndef C5DBRECORD_H
#define C5DBRECORD_H

#include "c5database.h"
#include <QObject>

class TableRecord {
public:
    QVariant id;
    inline QString _id() {return id.toString(); }
    virtual bool write(C5Database &db, QString &err) = 0;
    virtual void bind(C5Database &db) = 0;
    virtual bool getRecord(C5Database &db);
    virtual TableRecord &queryRecordOfId(C5Database &db, const QVariant &id);
    bool getWriteResult(C5Database &db, bool result, QString &err);
    bool insert(C5Database &db, const QString &table, QString &err);
    bool insertWithId(C5Database &db, const QString &table, QString &err);
    bool update(C5Database &db, const QString &table, QString &err);
};


#endif // C5DBRECORD_H
