#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QTcpSocket>

class QTcpSocket;

class SocketThread : public QObject
{
    Q_OBJECT

public:
    SocketThread(QTcpSocket *socket, QObject *parent = nullptr);
    ~SocketThread();

public slots:
    void sendData(const QString &uuid);

private:
    QString fUUID;
    QTcpSocket *fSocket;
    int fDataSize;
    QByteArray fData;
    void processJson();
    void jsonAuth(QJsonObject &jo);
    void jsonAuthUser(QJsonObject &jo);
    void jsonDisconnect(QJsonObject &jo);
    void write();

private slots:
    void disconnected();
    void error(QAbstractSocket::SocketError err);
    void readyRead();

signals:
    void registerConnection(const QString &uuid, const QString &inout, const QString &remoteIp, const QString &host, const QString &user);
    void unregisterConnection(const QString &uuid, const QString &reason);
    void connectionRXTX(const QString &uuid, int rx, int tx);
    void socketDisconnected(QTcpSocket *);
    void dataReady(const QString &, QByteArray &);
};

extern QString hostPassword;
extern QString dbHost;
extern int dbPort;
extern QString dbFile;
extern QString dbUser;
extern QString dbPass;

#endif // SOCKETTHREAD_H
