#ifndef DBDATA_H
#define DBDATA_H

#include "c5database.h"

class DbData
{
public:
    DbData(const QString &tableName);

    static void setDBParams(const QStringList &dbParams);

protected:
    QMap<int, QMap<QString, QVariant> > fData;

    QVariant get(int id, const QString &key);

    void getFromDatabase(const QString &tableName);

private:
    C5Database fDb;

    static QStringList fDbParams;
};

#endif // DBDATA_H
