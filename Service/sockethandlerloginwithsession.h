#ifndef SOCKETHANDLERLOGINWITHSESSION_H
#define SOCKETHANDLERLOGINWITHSESSION_H

#include "sockethandler.h"

class SocketHandlerLoginWithSession : public SocketHandler
{
public:
    SocketHandlerLoginWithSession(QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERLOGINWITHSESSION_H
