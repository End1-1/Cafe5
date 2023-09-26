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
    void listOfTask();
    void listOfManager();
    void listOfEmployee();
    void listOfWorks();
    void listOfTaskWorks();
    void addWorkToTask();
    void employesOfDay();
    void addWorkerToDay();
    void changeQty();
    void removeWork();
    void removeWorker();
    void workDetails();
    void workDetailsList();
    void workDetailsUpdate();
    void workDetailsDone();
    void workDetailsDoneUpdate();
    void removeWorkDetails();
    void workDetailsUndone();
};

#endif // QUERYJSONRESPONSE_H

