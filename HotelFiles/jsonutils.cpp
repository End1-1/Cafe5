#include "jsonutils.h"
#include <QJsonObject>

QJsonArray fetchFromDb(C5Database &db)
{
    QJsonArray ja;
    while (db.nextRow()) {
        QJsonObject jdb;
        for (int i = 0; i < db.columnCount(); i++) {
            jdb[db.columnName(i)] = db.getString(i);
        }
        ja.append(jdb);
    }
    return ja;
}
