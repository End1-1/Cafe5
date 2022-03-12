#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include "sslsocket.h"
#include "structs.h"
#include <QTimer>

class SocketConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnection(const QString &phone, const QString &uuid, QObject *parent = nullptr);
    ~SocketConnection();

public slots:
    void run();
    void sendData(QByteArray &d);    
    void driverRoute(SocketConnection *s, const QString &filename);

private:
    static SocketConnection *fInstance;
    quint32 fTcpPacketNumber;
    QString fPhone;
    QString fUuid;
    QByteArray fData;
    quint32 fMessageNumber;
    quint32 fMessageId;
    quint32 fMessageSize;
    quint16 fMessageCommand;
    SslSocket *fSocket;
    QTimer *fTimer;
    QTimer *fRouteTimer;
    int fRouteId;
    int fRouteDisconnectTimeout;
    quint32 getTcpPacketNumber();
    QList<CoordinateData> fRoute;

private slots:
    void timeout();
    void routeTimeout();
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
