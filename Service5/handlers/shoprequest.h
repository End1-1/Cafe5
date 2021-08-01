#ifndef SHOPREQUEST_H
#define SHOPREQUEST_H

#include "requesthandler.h"
#include "database.h"
#include <QJsonObject>

class ShopRequest : public RequestHandler
{
    Q_OBJECT

public:
    ShopRequest();

protected:
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;

private:
    QJsonObject fOrderJson;
    bool validateMultipartFormData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    bool validateApplicationJson(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    bool writeOutput(const QString &docId, QString &err, Database &db);
    bool writeMovement(const QString &orderid, int goods, double qty, int srcStore, Database &db);
    void checkQty(const QString &orderid, Database &db);

};

#endif // SHOPREQUEST_H
