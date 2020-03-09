#include "sockethandler.h"
#include "servicecommands.h"

SocketHandler::SocketHandler(QByteArray &data) :
    fData(data)
{
    fResponseCode = dr_ok;
}

bool SocketHandler::closeConnection()
{
    return true;
}
