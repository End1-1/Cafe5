#include "sockethandlerunknown.h"
#include "servicecommands.h"

SocketHandlerUnknown::SocketHandlerUnknown(QByteArray &data) :
    SocketHandler(data)
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
