#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include <QObject>

class QHttpServer;
class QTcpServer;

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer();

    ~C5WaiterServer();

    void reply(QJsonObject &o);

    int cmd();

private:

    QTcpServer *mTcpServer;
    QHttpServer *mServer;

};

#endif // C5WAITERSERVER_H
