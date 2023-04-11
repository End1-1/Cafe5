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
    int fUserId;

    bool dbFail();
    void dbToArray(Database &db, QJsonArray &ja);
    bool checkAPIandFCMToken();
    void registerDevice();
    void login();
    bool downloadData();
    void checkHashOfPass();
    bool stock();
    void saveOrder();
    void preordersList();
    void preorderDetails();
    void getDebts();
    bool orderList();
    bool getRoute();
};

#endif // JSONREPONSE_H
