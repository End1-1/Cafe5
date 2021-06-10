#ifndef SHOPREQUEST_H
#define SHOPREQUEST_H

#include "requesthandler.h"
#include "database.h"
#include <QJsonObject>

class ShopRequest : public RequestHandler
{
public:
    ShopRequest();

protected:
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;

private:
    QJsonObject fOrderJson;
    static QMap<int, int> fShopCounters;
    bool genOrderId(Database &db, QString &uuid, QString &userid, QString &err);
    bool validateMultipartFormData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    bool validateApplicationJson(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
};

#endif // SHOPREQUEST_H
