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

private slots:
    void disconnected();
    void error(QAbstractSocket::SocketError err);
    void readyRead();

signals:
    void socketDisconnected(QTcpSocket *);
    void dataReady(const QString &, QByteArray &);
};

#endif // SOCKETTHREAD_H
