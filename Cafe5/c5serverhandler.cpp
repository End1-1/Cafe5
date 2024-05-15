#include "c5serverhandler.h"
#include "c5config.h"
#include <QJsonArray>

C5ServerHandler::C5ServerHandler() :
    fDb(C5Config::dbParams())
{
    fDb.open();
}

void C5ServerHandler::getJsonFromQuery(const QString &query, QJsonArray &arr, const QMap<QString, QVariant> &bind)
{
    while (arr.count() > 0) {
        arr.removeAt(0);
    }
    for (QMap<QString, QVariant>::const_iterator it = bind.begin(); it != bind.end(); it++) {
        fDb[it.key()] = it.value();
    }
    fDb.exec(query);
    while (fDb.nextRow()) {
        QJsonObject o;
        for (int i = 0; i < fDb.columnCount(); i++) {
            o[fDb.columnName(i)] = fDb.getString(i);
        }
        arr.append(o);
    }
}
