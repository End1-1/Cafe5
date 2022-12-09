#ifndef SHOPCLIENTHANDLER_H
#define SHOPCLIENTHANDLER_H

#include "rawmessage.h"
#include "shopconnection.h"

class ShopClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ShopClientHandler(QHash<QString, QString> conn, QObject *parent = nullptr);

    ~ShopClientHandler();

    void run(RawMessage *rm, const QByteArray &in);

    void removeSocket(SslSocket *s);

private:
    int fTaskCounter;

    QMap<QString, ShopConnection*> fConnections;

    QHash<QString, QString> fDbConnectionSettings;

private slots:
    void tasks();

    void broadcastForClients(QByteArray data);

    void removeMeFromConnectionList(QString uuid);

signals:
    void handleData(RawMessage*, const QByteArray);

    void removeSocketHolder(SslSocket *);

    void sendToAllClients(QByteArray);

};

#endif // SHOPCLIENTHANDLER_H
