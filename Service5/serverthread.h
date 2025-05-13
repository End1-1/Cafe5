#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QObject>

class QWebSocketServer;
class QWebSocket;

enum SocketType {unknown = 0, hotel, waiter, smart, kinopark, cafemobile, officen, shop, server5};

struct SocketStruct {
    QWebSocket *socket;
    /*
     * 0 - unknown
     * 1 - hotel
     * 2 - waiter
     * 3 - smart
     * 4 - kinopark
     * 5 - cafemobile
     * 6 - officen
     * 7 - shop
     * 8 - server5 (kinopark printing)
     */
    int socketType;
    int userid = 0;
    QString database;
};

class ServerThread : public QObject
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);
    ~ServerThread();

public slots:
    void run();

private:
    QWebSocketServer *fServer;
    const QString fConfigPath;
    QList<SocketStruct> fSockets;
    QStringList fDbList;
    QString getDbList(const QJsonObject &jdoc);
    QString getConnection(const QJsonObject &jdoc);
    QString handleDll(const QJsonObject &jdoc, const QString &command);
    void registerSocket(const QJsonObject &jdoc, QWebSocket *ws);
    void unregisterSocket(const QJsonObject &jdoc, QWebSocket *ws);
    QString updateHotelCache(const QJsonObject &jdoc);
    QString armsoft(const QJsonObject &jdoc);
    void handleCommand(const QJsonObject &jdoc, QString &repMsg);

private slots:
    void onNewConnection();
    void onDisconnected();
    void onTextMessage(const QString &msg);

signals:
    void finished();

};

#endif // SERVERTHREAD_H
