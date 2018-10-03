#include "c5serverhandler.h"
#include "c5connection.h"
#include <QJsonArray>

C5ServerHandler::C5ServerHandler()
{
    fDb.open();
}

void C5ServerHandler::getJsonFromQuery(const QString &query, QJsonArray &arr, const QMap<QString, QVariant> &bind)
{
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
