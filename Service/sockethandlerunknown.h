#ifndef SOCKETHANDLERUNKNOWN_H
#define SOCKETHANDLERUNKNOWN_H

#include "sockethandler.h"

class SocketHandlerUnknown : public SocketHandler
{
public:
    SocketHandlerUnknown(QByteArray &data);
    virtual void processData() override;
    virtual bool closeConnection() override;
};

#endif // SOCKETHANDLERUNKNOWN_H
