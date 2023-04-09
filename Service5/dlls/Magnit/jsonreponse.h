#ifndef JSONREPONSE_H
#define JSONREPONSE_H

#include "database.h"
#include <QJsonArray>
#include <QJsonObject>

class JsonReponse
{
public:
    JsonReponse(Database &db, const QJsonObject &ji, QJsonObject &jo);
    void getResponse();
private:
    Database &fDb;
    const QJsonObject &fJsonIn;
    QJsonObject &fJsonOut;

    int fDeviceId;

    void dbToArray(Database &db, QJsonArray &ja);
    bool checkAPIandFCMToken();
    void registerDevice();
    void login();
    void downloadData();
    void checkHashOfPass();
    void stock();
    void saveOrder();
};

#endif // JSONREPONSE_H
