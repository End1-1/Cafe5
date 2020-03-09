#ifndef SOCKETHANDLERLOGIN_H
#define SOCKETHANDLERLOGIN_H

#include "sockethandler.h"

class SocketHandlerLogin : public SocketHandler
{
public:
    SocketHandlerLogin(QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERLOGIN_H
