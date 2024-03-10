#ifndef QUERYJSONRESPONSE_H
#define QUERYJSONRESPONSE_H

#include <QJsonArray>
#include <QJsonObject>


class QueryJsonResponse
{
public:
    QueryJsonResponse(const QString &host, const QJsonObject &ji, QJsonObject &jo);
    const QJsonObject &fJsonIn;
    QJsonObject &fJsonOut;
    void getResponse();

private:
    bool dbQuery();
    bool networkRedirect(const QString &sql);
    //bool printBill(const QString &id);
    QJsonObject fConfig;
    QString fHost;
};

#endif // QUERYJSONRESPONSE_H

