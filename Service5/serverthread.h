#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "socketstruct.h"
#include <QObject>

class QWebSocketServer;
class QWebSocket;

class ServerThread : public QObject
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);
    ~ServerThread();

public slots:
    void run();

private:
    QWebSocketServer* fServer;
    const QString fConfigPath;
    QHash<QString, QString> mDatabases;
    QHash<QWebSocket*, SocketStruct> fSockets;
    QStringList fDbList;
    QString getDbList(const QJsonObject &jdoc);
    QString getConnection(const QJsonObject &jdoc);
    QString handleDll(const QJsonObject &jdoc, const QString &command);
    void registerSocket(const QJsonObject &jdoc, QWebSocket *ws);
    void unregisterSocket(const QJsonObject &jdoc, QWebSocket *ws);
    QString updateHotelCache(const QJsonObject &jdoc);
    QString armsoft(const QJsonObject &jdoc);
    void handleCommand(SocketStruct ws, const QJsonObject &jdoc, QString &repMsg);

private slots:
    void onNewConnection();
    void onDisconnected();
    void onTextMessage(const QString &msg);
    void onBinaryMessage(const QByteArray &msg);

signals:
    void finished();

};

#endif // SERVERTHREAD_H
