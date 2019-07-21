#include "c5jsondb.h"
#include "c5database.h"
#include <QJsonObject>
#include <QJsonArray>

C5JsonDb::C5JsonDb()
{

}

QJsonObject C5JsonDb::convertRowToJsonObject(C5Database &db)
{
    QJsonObject o;
    if (db.nextRow()) {
        for (int i = 0; i < db.columnCount(); i++) {
            o[db.columnName(i)] = db.getString(i);
        }
    }
    return o;
}

QJsonArray C5JsonDb::convertRowsToJsonArray(C5Database &db)
{
    QJsonArray ja;
    while (db.nextRow()) {
        QJsonObject o;
        for (int i = 0; i < db.columnCount(); i++) {
            o[db.columnName(i)] = db.getString(i);
        }
        ja.append(o);
    }
    return ja;
}
