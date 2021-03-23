#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "requesthandler.h"

class Registration : public RequestHandler
{
public:
    Registration();

protected:
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
};

#endif // REGISTRATION_H
