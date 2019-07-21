#ifndef C5JSONDB_H
#define C5JSONDB_H

class C5Database;
class QJsonObject;
class QJsonArray;

class C5JsonDb
{
public:
    C5JsonDb();

    static QJsonObject convertRowToJsonObject(C5Database &db);

    static QJsonArray convertRowsToJsonArray(C5Database &db);
};

#endif // C5JSONDB_H
