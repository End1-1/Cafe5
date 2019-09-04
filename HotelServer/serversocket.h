#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <QObject>

class QTcpServer;

class ServerSocket : public QObject
{
    Q_OBJECT
public:
    explicit ServerSocket(QObject *parent = nullptr);
    bool listen(quint16 port);

private:
    QTcpServer *fServer;

private slots:
    void newConnection();
};

#endif // SERVERSOCKET_H
