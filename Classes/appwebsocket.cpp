#include "appwebsocket.h"
#include "logwriter.h"
#include <QAbstractSocket>
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>

QString AppWebSocket::mHost;
AppWebSocket* AppWebSocket::instance = nullptr;

namespace {

QString socketStateName(QAbstractSocket::SocketState state)
{
    switch(state) {
    case QAbstractSocket::UnconnectedState: return QStringLiteral("Unconnected");
    case QAbstractSocket::HostLookupState: return QStringLiteral("HostLookup");
    case QAbstractSocket::ConnectingState: return QStringLiteral("Connecting");
    case QAbstractSocket::ConnectedState: return QStringLiteral("Connected");
    case QAbstractSocket::BoundState: return QStringLiteral("Bound");
    case QAbstractSocket::ListeningState: return QStringLiteral("Listening");
    case QAbstractSocket::ClosingState: return QStringLiteral("Closing");
    }
    return QStringLiteral("Unknown(%1)").arg(static_cast<int>(state));
}

QString socketErrorName(QAbstractSocket::SocketError error)
{
    switch(error) {
    case QAbstractSocket::ConnectionRefusedError: return QStringLiteral("ConnectionRefused");
    case QAbstractSocket::RemoteHostClosedError: return QStringLiteral("RemoteHostClosed");
    case QAbstractSocket::HostNotFoundError: return QStringLiteral("HostNotFound");
    case QAbstractSocket::SocketAccessError: return QStringLiteral("SocketAccess");
    case QAbstractSocket::SocketResourceError: return QStringLiteral("SocketResource");
    case QAbstractSocket::SocketTimeoutError: return QStringLiteral("SocketTimeout");
    case QAbstractSocket::DatagramTooLargeError: return QStringLiteral("DatagramTooLarge");
    case QAbstractSocket::NetworkError: return QStringLiteral("Network");
    case QAbstractSocket::AddressInUseError: return QStringLiteral("AddressInUse");
    case QAbstractSocket::SocketAddressNotAvailableError: return QStringLiteral("AddressNotAvailable");
    case QAbstractSocket::UnsupportedSocketOperationError: return QStringLiteral("UnsupportedOperation");
    case QAbstractSocket::UnfinishedSocketOperationError: return QStringLiteral("UnfinishedOperation");
    case QAbstractSocket::ProxyAuthenticationRequiredError: return QStringLiteral("ProxyAuthRequired");
    case QAbstractSocket::SslHandshakeFailedError: return QStringLiteral("SslHandshakeFailed");
    case QAbstractSocket::ProxyConnectionRefusedError: return QStringLiteral("ProxyConnectionRefused");
    case QAbstractSocket::ProxyConnectionClosedError: return QStringLiteral("ProxyConnectionClosed");
    case QAbstractSocket::ProxyConnectionTimeoutError: return QStringLiteral("ProxyConnectionTimeout");
    case QAbstractSocket::ProxyNotFoundError: return QStringLiteral("ProxyNotFound");
    case QAbstractSocket::ProxyProtocolError: return QStringLiteral("ProxyProtocol");
    case QAbstractSocket::OperationError: return QStringLiteral("Operation");
    case QAbstractSocket::SslInternalError: return QStringLiteral("SslInternal");
    case QAbstractSocket::SslInvalidUserDataError: return QStringLiteral("SslInvalidUserData");
    case QAbstractSocket::TemporaryError: return QStringLiteral("Temporary");
    case QAbstractSocket::UnknownSocketError: return QStringLiteral("Unknown");
    }
    return QStringLiteral("Error(%1)").arg(static_cast<int>(error));
}

} // namespace

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

    mReconnectTimer = new QTimer(this);
    mReconnectTimer->setSingleShot(true);
    connect(mReconnectTimer, &QTimer::timeout, this, &AppWebSocket::connectToServer);

    mPingTimer = new QTimer(this);
    connect(mPingTimer, &QTimer::timeout, this, &AppWebSocket::pingServer);
    mPingTimer->start(10000);

    connectToServer();
}

AppWebSocket::~AppWebSocket()
{
    stopReconnect();
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

    instance->stopReconnect();
    instance->mReconnectAttempt = 0;

    if(instance->mSocket->state() != QAbstractSocket::UnconnectedState) {
        instance->mSuppressReconnect = true;
        instance->mSocket->abort();
        instance->mSuppressReconnect = false;
    }
    instance->mConnectionState = disconnected;

    instance->mHost = host;
    instance->mServerKey = key;
    instance->mUsername = username;
    instance->mPassword = password;
    instance->connectToServer();
}

bool AppWebSocket::isConnected() const
{
    return mSocket && mSocket->state() == QAbstractSocket::ConnectedState;
}

bool AppWebSocket::sendMessage(const QString &message)
{
    qDebug() << "sending message" << message;
    if(!isConnected()) {
        qDebug() << "sendMessage: not connected";
        return false;
    }
    const qint64 n = mSocket->sendTextMessage(message);
    if(n < 0 || (n == 0 && !message.isEmpty())) {
        qDebug() << "sendTextMessage failed" << mSocket->errorString();
        return false;
    }
    return true;
}

bool AppWebSocket::sendBinaryMessage(const QByteArray &ba)
{
    qDebug() << "sending binary message" << ba.size() << "bytes";
    if(!isConnected()) {
        qDebug() << "sendBinaryMessage: not connected";
        return false;
    }
    const qint64 n = mSocket->sendBinaryMessage(ba);
    if(n < 0 || (n == 0 && !ba.isEmpty())) {
        qDebug() << "sendBinaryMessage failed" << mSocket->errorString();
        return false;
    }
    return true;
}

bool AppWebSocket::sendMessage(const QJsonObject &json)
{
    return sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

int AppWebSocket::reconnectDelayMs() const
{
    // 2s, 4s, 8s ... capped at 30s
    const int base = 2000;
    const int cappedAttempt = qMin(mReconnectAttempt, 4);
    return qMin(base * (1 << cappedAttempt), 30000);
}

void AppWebSocket::stopReconnect()
{
    if(mReconnectTimer) {
        mReconnectTimer->stop();
    }
}

void AppWebSocket::logFailure(const QString &reason)
{
    const QString msg = QStringLiteral(
        "%1 | host=%2 | attempt=%3 | state=%4 | socketError=%5 | errorString=%6 | closeCode=%7 | closeReason=%8")
                            .arg(reason,
                                 mHost.isEmpty() ? QStringLiteral("(empty)") : mHost,
                                 QString::number(mReconnectAttempt),
                                 socketStateName(mSocket->state()),
                                 socketErrorName(mSocket->error()),
                                 mSocket->errorString().isEmpty() ? QStringLiteral("-") : mSocket->errorString(),
                                 QString::number(static_cast<int>(mSocket->closeCode())),
                                 mSocket->closeReason().isEmpty() ? QStringLiteral("-") : mSocket->closeReason());
    LogWriter::write(LogWriterLevel::websocket, QStringLiteral("WS"), msg);
    qDebug() << msg;
}

void AppWebSocket::scheduleReconnect()
{
    if(mHost.isEmpty()) {
        return;
    }
    if(isConnected()) {
        return;
    }
    if(mReconnectTimer->isActive()) {
        return;
    }
    const int delay = reconnectDelayMs();
    qDebug() << "websocket reconnect in" << delay << "ms (attempt" << mReconnectAttempt << ")";
    mReconnectTimer->start(delay);
}

void AppWebSocket::connectToServer()
{
    qDebug() << "connecting to server" << mHost;

    if(mHost.isEmpty()) {
        logFailure(QStringLiteral("connect_skipped_empty_host"));
        return;
    }

    if(isConnected()) {
        return;
    }

    stopReconnect();

    // Drop a stuck Connecting/Closing socket so open() can run again
    if(mSocket->state() != QAbstractSocket::UnconnectedState) {
        mSuppressReconnect = true;
        mSocket->abort();
        mSuppressReconnect = false;
    }

    mConnectionState = connecting;
    emit socketConnecting();
    mSocket->open(QUrl(mHost));
}

void AppWebSocket::pingServer()
{
    if(mConnectionState == connected && isConnected()) {
        sendMessage("ping");
        return;
    }
    // Keep trying forever while offline
    scheduleReconnect();
}

void AppWebSocket::connectedToServer()
{
    qDebug() << "connected to server";
    if(mReconnectAttempt > 0) {
        LogWriter::write(LogWriterLevel::websocket, QStringLiteral("WS"),
                         QStringLiteral("connected_after_failures | host=%1 | failedAttempts=%2")
                             .arg(mHost)
                             .arg(mReconnectAttempt));
    }
    stopReconnect();
    mReconnectAttempt = 0;
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
    if(mSuppressReconnect) {
        return;
    }
    ++mReconnectAttempt;
    logFailure(QStringLiteral("disconnected"));
    scheduleReconnect();
}

void AppWebSocket::socketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << "websocket error" << mSocket->errorString();
    mConnectionState = disconnected;
    emit socketDisconnected();
    logFailure(QStringLiteral("socket_error"));
    // Safety net if disconnected signal is delayed/missing
    scheduleReconnect();
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
