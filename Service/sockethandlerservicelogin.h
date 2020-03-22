#ifndef SOCKETHANDLERSERVICELOGIN_H
#define SOCKETHANDLERSERVICELOGIN_H

#include "sockethandler.h"

class SocketHandlerServiceLogin : public SocketHandler
{
public:
    SocketHandlerServiceLogin(SocketData *sd, QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERSERVICELOGIN_H
