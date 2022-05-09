#ifndef TABLERECORD_H
#define TABLERECORD_H

#include "database.h"
#include <QHash>

class TableRecord
{
public:
    TableRecord(const QString &table, Database &db);
    QVariant &operator [](const QString &name);
    bool insert();
    bool insert(int &id);
    bool update(const QString &fieldName, const QVariant &fieldValue);

private:
    const QString &fTableName;
    Database &fDb;
    QHash<QString, QHash<QString, QVariant>> fValues;
    void bindValues();
    void clearValues();
};

#endif // TABLERECORD_H
