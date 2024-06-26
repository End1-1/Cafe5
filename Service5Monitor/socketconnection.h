#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include "sslsocket.h"
#include <QTimer>

class SocketConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnection(QObject *parent = nullptr);
    ~SocketConnection();
    static void init();
    static SocketConnection *instance();

public slots:
    void run();
    void sendData(QByteArray &d);

private:
    static SocketConnection *fInstance;
    static quint32 fTcpPacketNumber;
    QByteArray fData;
    quint32 fMessageNumber;
    quint32 fMessageId;
    quint32 fMessageSize;
    quint16 fMessageCommand;
    SslSocket *fSocket;
    QTimer *fTimer;
    quint32 getTcpPacketNumber();

private slots:
    void timeout();
    void encrypted();
    void disconnected();
    void readyRead();
    void errorOccurred(QAbstractSocket::SocketError error);

signals:
    void stopped();
    void connected();
    void connectionLost();
    void dataReady(QByteArray &);
    void externalDataReady(quint16, const QByteArray &);
};

#endif // SOCKETCONNECTION_H
