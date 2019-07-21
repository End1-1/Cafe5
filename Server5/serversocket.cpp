#include "serversocket.h"
#include "socketthread.h"
#include <QTcpServer>

ServerSocket::ServerSocket(QObject *parent) : 
    QObject(parent)
{
    fServer = new QTcpServer(this);
    connect(fServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

bool ServerSocket::listen(quint16 port)
{
    return fServer->listen(QHostAddress::Any, port);
}

void ServerSocket::newConnection()
{
    QTcpSocket *s = fServer->nextPendingConnection();
    SocketThread *st = new SocketThread(s, this);
    connect(st, SIGNAL(dataReady(QString, QByteArray&)), parent(), SLOT(clientSocketDataRead(QString, QByteArray&)));
    connect(parent(), SIGNAL(sendData(QString)), st, SLOT(sendData(QString)));
}
