#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "requesthandler.h"

class Authentication : public RequestHandler
{
public:
    Authentication();
    ~Authentication();

protected:
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
};

#endif // AUTHENTICATION_H
