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

    QString fbDb();
    void returnQueryResult(const QString &queryName);

    void getCafeList();
    void getStoreList();
    void getBaseItems();
    void getAmtStorageList();
    void getAsStorageList();
    void getStorageItems();
    void openInventoryDoc();
    void updateInventoryDocItem();
    void getGoodsQrAndWeight();
    void getGoodsNames();
    void saveGoodsQrName();
    void getConfig();
    void getAragamash();
    void getAllTogether();
};

#endif // QUERYJSONRESPONSE_H

