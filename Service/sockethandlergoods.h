#ifndef SOCKETHANDLERGOODS_H
#define SOCKETHANDLERGOODS_H

#include "sockethandler.h"

class SocketHandlerGoods : public SocketHandler
{
public:
    SocketHandlerGoods(QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERGOODS_H
