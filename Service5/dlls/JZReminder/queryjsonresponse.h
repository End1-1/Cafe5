#ifndef QUERYJSONRESPONSE_H
#define QUERYJSONRESPONSE_H

#include "database.h"
#include <QJsonArray>
#include <QJsonObject>


class QueryJsonResponse
{
public:
    QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo);
    Database &fDb;
    const QJsonObject &fJsonIn;
    QJsonObject &fJsonOut;
    void getResponse();

private:
    void dbToArray(QJsonArray &ja);
    bool getList();
    bool updateState();
    bool returnFail(const QString &err);
};

#endif // QUERYJSONRESPONSE_H

