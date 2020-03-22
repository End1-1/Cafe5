#include "sockethandlerlocationchanged.h"

SocketHandlerLocationChanged::SocketHandlerLocationChanged(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerLocationChanged::processData()
{
    qDebug() << fData;
}
