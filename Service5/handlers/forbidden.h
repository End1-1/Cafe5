#ifndef FORBIDDEN_H
#define FORBIDDEN_H

#include "requesthandler.h"

class Forbidden : public RequestHandler
{
public:
    Forbidden();

protected:
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
};

#endif // FORBIDDEN_H
