#include "socketthread.h"
#include <QUuid>

SocketThread::SocketThread(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    fDataSize = 0;
    fUUID = QUuid::createUuid().toByteArray();
    qRegisterMetaType <QAbstractSocket::SocketError> ("QAbstractSocket::SocketState");
    fSocket = socket;
    connect(fSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(fSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

SocketThread::~SocketThread()
{
    qDebug() << "SocketThread destructor";
}

void SocketThread::sendData(const QString &uuid)
{
    if (fUUID != uuid) {
        return;
    }
    fDataSize = fData.length();
    fSocket->write(reinterpret_cast<const char *>(&fDataSize), sizeof(fDataSize));
    fSocket->write(fData);
    fSocket->flush();
    fData.clear();
    fDataSize = 0;
}

void SocketThread::disconnected()
{
    emit socketDisconnected(fSocket);
}

void SocketThread::error(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}

void SocketThread::readyRead()
{
    if (fDataSize == 0) {
        fSocket->read(reinterpret_cast<char *>(&fDataSize), sizeof(fDataSize));
    }
    if (fSocket->bytesAvailable()) {
        fData.append(fSocket->readAll());
    }
    if (fData.length() >= fDataSize) {
        emit dataReady(fUUID, fData);
    }
}
