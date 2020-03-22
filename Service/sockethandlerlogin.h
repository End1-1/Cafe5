#ifndef SOCKETHANDLERLOGIN_H
#define SOCKETHANDLERLOGIN_H

#include "sockethandler.h"

class SocketHandlerLogin : public SocketHandler
{
public:
    SocketHandlerLogin(SocketData *sd, QByteArray &data);
    bool login();
    virtual void processData() override;
};

#endif // SOCKETHANDLERLOGIN_H
