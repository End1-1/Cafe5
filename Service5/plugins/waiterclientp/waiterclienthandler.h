#ifndef WAITERCLIENTHANDLER_H
#define WAITERCLIENTHANDLER_H

#include "rawmessage.h"
#include "waiterconnection.h"

class WaiterClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit WaiterClientHandler(QHash<QString, QString> conn, QObject *parent = nullptr);

    ~WaiterClientHandler();

    void run(RawMessage *rm, const QByteArray &in);

    void removeSocket(SslSocket *s);

private:
    int fTaskCounter;

    QMap<QString, WaiterConnection*> fConnections;

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

#endif // WAITERCLIENTHANDLER_H
