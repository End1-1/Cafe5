#ifndef STOREREQUEST_H
#define STOREREQUEST_H

#include "requesthandler.h"

class StoreRequest : public RequestHandler
{
public:
    StoreRequest();

protected:
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;

private:
    QStringList fGoods;
    QString fQuery;
};

#endif // STOREREQUEST_H
