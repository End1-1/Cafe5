#ifndef SOCKETHANDLERUUID_H
#define SOCKETHANDLERUUID_H

#include "sockethandler.h"

class SocketHandlerUUID : public SocketHandler
{
public:
    SocketHandlerUUID(SocketData *sd, QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERUUID_H
