#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QMap>

class QTcpSocket;

class ClientSocket : public QObject
{
    Q_OBJECT

public:
    ClientSocket();
    bool connectToSocket(const QString &ip, int port, QJsonObject &jo);
    void disconnectFromHost();
    QVariant &operator[](const QString &name);
    bool sendJson();
    QJsonObject responseJson() const;

private:
    QTcpSocket *fSocket;
    QByteArray fBuff;
    QMap<QString, QVariant> fBindValues;
    QString fUUID;
};

extern ClientSocket __socket;

#endif // CLIENTSOCKET_H
