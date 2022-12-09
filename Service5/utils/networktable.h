#ifndef NETWORKTABLE_H
#define NETWORKTABLE_H

#include "rawmessage.h"
#include "database.h"

class NetworkTable
{
public:
    NetworkTable(RawMessage &rm, Database &db);
    void execSQL(const QString &sql);
    RawMessage &fRM;
    Database &fDb;
    int fRowCount;

private:
    QStringList fStrings;
};

#endif // NETWORKTABLE_H
