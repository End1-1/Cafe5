#ifndef C5SERVERHANDLER_H
#define C5SERVERHANDLER_H

#include "c5database.h"

class C5ServerHandler
{
public:
    C5ServerHandler();
    void getJsonFromQuery(const QString &query, QJsonArray &arr, const QMap<QString, QVariant> &bind = QMap<QString, QVariant>());
    C5Database fDb;
};

#endif // C5SERVERHANDLER_H
