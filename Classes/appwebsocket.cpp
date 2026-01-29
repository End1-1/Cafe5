#include "appwebsocket.h"
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>

QString AppWebSocket::mHost;
AppWebSocket* AppWebSocket::instance = nullptr;

AppWebSocket::AppWebSocket(QObject *parent)
    : QObject{parent},
      mConnectionState(disconnected)
{
    mSocket = new QWebSocket();
    connect(mSocket, &QWebSocket::connected, this, &AppWebSocket::connectedToServer);
    connect(mSocket, &QWebSocket::disconnected, this, &AppWebSocket::disconnectedFromServer);
    connect(mSocket, &QWebSocket::errorOccurred, this, &AppWebSocket::socketError);
    connect(mSocket, &QWebSocket::textMessageReceived, this, &AppWebSocket::textMessageReceived);
    connect(mSocket, &QWebSocket::binaryMessageReceived, this, &AppWebSocket::binaryMessageReceived);
    connectToServer();
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AppWebSocket::pingServer);
    timer->start(10000);
}

AppWebSocket::~AppWebSocket()
{
    mSocket->deleteLater();
}

void AppWebSocket::initInstance()
{
    instance = new AppWebSocket();
}

void AppWebSocket::reconnect(const QString &host, const QString &key, const QString &username, const QString &password)
{
    if(!instance) {
        initInstance();
    }

    if(instance->mConnectionState == connected) {
        instance->disconnectedFromServer();
    }

    instance->mHost = host;
    instance->mServerKey = key;
    instance->mUsername = username;
    instance->mPassword = password;
    instance->connectToServer();
}

void AppWebSocket::sendMessage(const QString &message)
{
    qDebug() << "sending message" << message;
    mSocket->sendTextMessage(message);
}

void AppWebSocket::sendBinaryMessage(const QByteArray &ba)
{
    qDebug() << "sending binary message" << ba.size() << "bytes";
    mSocket->sendBinaryMessage(ba);
}

void AppWebSocket::sendMessage(const QJsonObject &json)
{
    sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void AppWebSocket::connectToServer()
{
    qDebug() << "connecting to server" << mHost;

    if(mHost.isEmpty()) {
        qDebug() << "Host is empty";
        return;
    }

    if(mConnectionState == connecting) {
        return;
    }

    mConnectionState = connecting;
    emit socketConnecting();
    QUrl url(mHost);
    mSocket->open(url);
}

void AppWebSocket::pingServer()
{
    if(mConnectionState == connected) {
        sendMessage("ping");
    } else {
        connectToServer();
    }
}

void AppWebSocket::connectedToServer()
{
    qDebug() << "connected to server";
    mConnectionState = connected;
    emit socketConnected();

    if(!mServerKey.isEmpty()) {
        QJsonObject jo = {
            {"command", "register_socket"},
            {"key", mServerKey},
            {"username", mUsername},
            {"password", mPassword}
        };
        sendMessage(jo);
    }
}

void AppWebSocket::disconnectedFromServer()
{
    qDebug() << "disconnected from websocket server";
    mConnectionState = disconnected;
    emit socketDisconnected();
}

void AppWebSocket::socketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << "websocket error" << mSocket->errorString();
    mSocket->close();
    emit socketDisconnected();
    QTimer::singleShot(3000, this, &AppWebSocket::connectToServer);
}

void AppWebSocket::textMessageReceived(const QString &message)
{
    qDebug() << "websocket message" << message.size();

    if(message.toLower() == "pong") {
        return;
    }

    emit messageReceived(message);
}

void AppWebSocket::binaryMessageReceived(const QByteArray &data)
{
    qDebug() << "binary message size:" << data.size();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if(err.error != QJsonParseError::NoError) {
        qWarning() << "JSON error:" << err.errorString();
        return;
    }

    emit bMessageReceived(doc.object());
}
