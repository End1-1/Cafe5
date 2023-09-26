#ifndef QUERYJSONRESPONSE_H
#define QUERYJSONRESPONSE_H

#include "database.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>

class QueryJsonResponse
{
public:
    QueryJsonResponse(QSettings &s, const QJsonObject &ji, QJsonObject &jo);
    const QJsonObject &fJsonIn;
    QJsonObject &fJsonOut;
    void getResponse();

private:
    void dbToArray(QJsonArray &ja, Database &db);
    QSettings &fSettings;

    void getCafeList();
    void getStoreList();
    void getBaseItems();
    void getAmtStorageList();

};

#endif // QUERYJSONRESPONSE_H

