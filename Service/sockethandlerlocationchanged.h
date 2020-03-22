#ifndef SOCKETHANDLERLOCATIONCHANGED_H
#define SOCKETHANDLERLOCATIONCHANGED_H

#include "sockethandler.h"

class SocketHandlerLocationChanged : public SocketHandler
{
public:
    SocketHandlerLocationChanged(SocketData *sd, QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERLOCATIONCHANGED_H
