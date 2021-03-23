#ifndef CONFIRMREGISTRATION_H
#define CONFIRMREGISTRATION_H

#include "requesthandler.h"

class ConfirmRegistration : public RequestHandler
{
public:
    ConfirmRegistration();
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
};

#endif // CONFIRMREGISTRATION_H
