#ifndef SHOPCONNECTION_H
#define SHOPCONNECTION_H

#include "rawmessage.h"


class ShopConnection : public QObject
{
    Q_OBJECT
public:
    explicit ShopConnection(QHash<QString,QString> conn, SslSocket *s);

    ~ShopConnection();

    int fUserId;

    QString fUserName;

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

#endif // SHOPCONNECTION_H
