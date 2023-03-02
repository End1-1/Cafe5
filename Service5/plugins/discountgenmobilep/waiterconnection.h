#ifndef WAITERCONNECTION_H
#define WAITERCONNECTION_H

#include "rawmessage.h"


class WaiterConnection : public QObject
{
    Q_OBJECT
public:
    explicit WaiterConnection(QHash<QString,QString> conn, SslSocket *s);

    ~WaiterConnection();

    int fUserId;

    QString fUserName;

    int fLockedTable;

    int fProtocolVersion;

    void sendToAllClients(QByteArray data);

public slots:
    void handleData(RawMessage *rm, const QByteArray &in);

    void removeSocket(SslSocket *s);

    void writeToSocket(QByteArray data);

private:
    SslSocket *fSocket;

    QHash<QString, QString> fDbConnectionSettings;

signals:
    void broadcastForClients(QByteArray);

    void removeMeFromConnectionList(QString);

};

#endif // WAITERCONNECTION_H
