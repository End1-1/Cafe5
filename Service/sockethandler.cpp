#include "sockethandler.h"
#include "servicecommands.h"
#include "sslsocket.h"
#include <QHostAddress>

SocketHandler::SocketHandler(QByteArray &data) :
    fData(data)
{
    fResponseCode = dr_ok;
}

void SocketHandler::setSocket(SslSocket *socket)
{
    fSslSocket = socket;
    fPeerAddress = QHostAddress(fSslSocket->peerAddress().toIPv4Address()).toString();
}

bool SocketHandler::closeConnection()
{
    return true;
}
