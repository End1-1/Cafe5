#ifndef TAX_H
#define TAX_H

#include "requesthandler.h"

class Tax : public RequestHandler
{
public:
    Tax();

protected:
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;

private:
    QString fAuthMethod;
    QString fKey;
    QString fUser;
    QString fPassword;
    QString fOrder;
};

#endif // TAX_H
