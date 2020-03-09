#ifndef SOCKETHANDLERSERVICECONFIG_H
#define SOCKETHANDLERSERVICECONFIG_H

#include "sockethandler.h"

class SocketHandlerServiceConfig : public SocketHandler
{
public:
    SocketHandlerServiceConfig(QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERSERVICECONFIG_H
