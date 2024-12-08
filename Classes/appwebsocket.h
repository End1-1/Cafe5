#ifndef APPWEBSOCKET_H
#define APPWEBSOCKET_H

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

class AppWebSocket : public QObject
{
    Q_OBJECT
public:
    enum ConnectionState {disconnected = 0, connecting, connected};

    explicit AppWebSocket(QObject *parent = nullptr);

    ConnectionState mConnectionState;

    static QString host;

    static AppWebSocket *instance;

    static void initInstance();

    void sendMessage(const QString &message);

    void sendMessage(const QJsonObject &json);

public slots:
    void connectToServer();

private:
    QWebSocket *mSocket;

private slots:
    void pingServer();

    void connectedToServer();

    void disconnectedFromServer();

    void socketError(QAbstractSocket::SocketError error);

    void textMessageReceived(const QString &message);

signals:
    void socketConnecting();

    void socketConnected();

    void socketDisconnected();
};

#endif // APPWEBSOCKET_H
