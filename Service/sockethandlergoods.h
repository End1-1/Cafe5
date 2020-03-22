#ifndef SOCKETHANDLERGOODS_H
#define SOCKETHANDLERGOODS_H

#include "sockethandler.h"

class SocketHandlerGoods : public SocketHandler
{
public:
    SocketHandlerGoods(SocketData *sd, QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERGOODS_H
