#ifndef SOCKETHANDLERUPLOADSALE_H
#define SOCKETHANDLERUPLOADSALE_H

#include "sockethandler.h"

class SocketHandlerUploadSale : public SocketHandler
{
public:
    SocketHandlerUploadSale(SocketData *sd, QByteArray &data);
    virtual void processData() override;
};

#endif // SOCKETHANDLERUPLOADSALE_H
