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

    ~AppWebSocket();

    ConnectionState mConnectionState;

    static QString mHost;

    static AppWebSocket* instance;

    static void initInstance();

    static void reconnect(const QString &host, const QString &key, const QString &username, const QString &password);

    bool isConnected() const;

    /// Returns false if not connected or the socket failed to send.
    bool sendMessage(const QString &message);

    bool sendBinaryMessage(const QByteArray &ba);

    bool sendMessage(const QJsonObject &json);

public slots:
    void connectToServer();

private:
    QWebSocket* mSocket;

    QString mServerKey;

    QString mUsername;

    QString mPassword;

private slots:
    void pingServer();

    void connectedToServer();

    void disconnectedFromServer();

    void socketError(QAbstractSocket::SocketError error);

    void textMessageReceived(const QString &message);

    void binaryMessageReceived(const QByteArray &data);

signals:
    void socketConnecting();

    void socketConnected();

    void socketDisconnected();

    void messageReceived(const QString &message);

    void bMessageReceived(const QJsonObject &jo);
};

#endif // APPWEBSOCKET_H
