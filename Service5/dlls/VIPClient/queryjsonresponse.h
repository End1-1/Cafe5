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
    QJsonArray dbToArray();
    bool dbFail(const QString &msg);
    bool init();
    bool callFunction(const QString &name);
    bool createOrder();
    bool dbQuery();
    bool networkRedirect(const QString &sql);
    bool payment();
    bool printBill();
    QJsonObject fConfig;
};

#endif // QUERYJSONRESPONSE_H

