#include "sockethandler.h"
#include "servicecommands.h"
#include "sslsocket.h"
#include <QHostAddress>

SocketHandler::SocketHandler(SocketData *sd, QByteArray &data) :
    fData(data)
{
    fResponseCode = dr_ok;
    fSocketData = sd;
    if (fSocketData) {
        fPeerAddress = QHostAddress(fSocketData->fSocket->peerAddress().toIPv4Address()).toString();
    }
}

bool SocketHandler::closeConnection()
{
    return fResponseCode != dr_ok;
}
