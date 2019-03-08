#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include <QObject>
#include <QJsonObject>
#include "c5database.h"

class QTcpSocket;

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer(const QJsonObject &o, QTcpSocket *socket);

    void reply(QJsonObject &o);

private:
    const QJsonObject &fIn;

    bool checkPermission(int user, int permission, C5Database &db);

    void saveOrder(QJsonObject &jh, QJsonArray &ja, C5Database &db);

    void saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db);

    int printTax(const QJsonObject &h, const QJsonArray &ja, C5Database &db);

    void closeOrderHotel(const QJsonObject &h);

    QTcpSocket *fSocket;
};

#endif // C5WAITERSERVER_H
