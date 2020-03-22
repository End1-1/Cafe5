#ifndef SOCKETHANDLERLOGINWITHSESSION_H
#define SOCKETHANDLERLOGINWITHSESSION_H

#include "sockethandler.h"

class SocketHandlerLoginWithSession : public SocketHandler
{
public:
    SocketHandlerLoginWithSession(SocketData *sd, QByteArray &data);
    bool login();
    virtual void processData() override;
};

#endif // SOCKETHANDLERLOGINWITHSESSION_H
