#include "sockethandlerunknown.h"
#include "servicecommands.h"

SocketHandlerUnknown::SocketHandlerUnknown(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerUnknown::processData()
{
    fResponseCode = dr_unknown_request;
    fData.clear();
    fData = "Unknown request";
}

bool SocketHandlerUnknown::closeConnection()
{
    return true;
}
