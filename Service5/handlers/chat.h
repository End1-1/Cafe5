#ifndef CHAT_H
#define CHAT_H

#include "requesthandler.h"

class Chat : public RequestHandler
{
public:
    Chat();

protected:
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);

private:
    QString fAuthMethod;
    QString fUser;
    QString fPassword;
    QString fAction;
};

#endif // CHAT_H
