#include "appwebsocket.h"
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>

QString AppWebSocket::host = "wss://kinopark.picasso.am/ws";
AppWebSocket *AppWebSocket::instance = nullptr;

AppWebSocket::AppWebSocket(QObject *parent)
    : QObject{parent},
      mConnectionState(disconnected)
{
    mSocket = new QWebSocket();
    connect(mSocket, &QWebSocket::connected, this, &AppWebSocket::connectedToServer);
    connect(mSocket, &QWebSocket::disconnected, this, &AppWebSocket::disconnectedFromServer);
    connect(mSocket, QOverload<QAbstractSocket::SocketError>::of( &QWebSocket::error), this, &AppWebSocket::socketError);
    connect(mSocket, &QWebSocket::textMessageReceived, this, &AppWebSocket::textMessageReceived);
    connectToServer();
    pingServer();
}

void AppWebSocket::initInstance()
{
    instance = new AppWebSocket();
}

void AppWebSocket::sendMessage(const QString &message)
{
    qDebug() << "sending message" << message;
    mSocket->sendTextMessage(message);
}

void AppWebSocket::sendMessage(const QJsonObject &json)
{
    sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void AppWebSocket::connectToServer()
{
    if (mConnectionState == connecting) {
        return;
    }
    mConnectionState = connecting;
    emit socketConnecting();
    QUrl url(host);
    mSocket->open(url);
    qDebug() << "connecting to server";
}

void AppWebSocket::pingServer()
{
    if (mConnectionState == connected) {
        sendMessage("ping");
    }
    QTimer::singleShot(5000, this, SLOT(pingServer()));
}

void AppWebSocket::connectedToServer()
{
    qDebug() << "connected to server";
    mConnectionState = connected;
    emit socketConnected();
}

void AppWebSocket::disconnectedFromServer()
{
    qDebug() << "disconnected from server";
    mConnectionState = disconnected;
    emit socketDisconnected();
    QTimer::singleShot(5000, this, SLOT(connectToServer()));
}

void AppWebSocket::socketError(QAbstractSocket::SocketError error)
{
    qDebug() << "websocket error" << mSocket->errorString();
    mSocket->close();
    QTimer::singleShot(5000, this, SLOT(connectToServer()));
}

void AppWebSocket::textMessageReceived(const QString &message)
{
    qDebug() << "websocket message" << message;
    if (message.toLower() == "pong") {
        return;
    }
}
