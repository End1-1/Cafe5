#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include "c5database.h"
#include "c5serverhandler.h"
#include <QObject>
#include <QJsonObject>

class QTcpSocket;

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer(const QJsonObject &o, QTcpSocket *socket);

    void reply(QJsonObject &o);

    int cmd();

private:
    const QJsonObject &fIn;

    QString fPeerAddress;

    bool checkPermission(int user, int permission, C5Database &db);

    void openActiveOrder(QJsonObject &jh, QJsonArray &jb, QJsonArray &jt, C5ServerHandler &srh);

    void saveOrder(QJsonObject &jh, QJsonArray &ja, C5Database &db);

    void saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db);

    int printTax(const QJsonObject &h, const QJsonArray &ja, C5Database &db);

    bool printBill(QJsonObject &jh, QJsonArray &jb, QString &err, C5ServerHandler &srh);

    bool printReceipt(QJsonObject &jh, QJsonArray &jb, QString &err, C5ServerHandler &srh);

    void remember(const QJsonObject &h);

    void getVersions(C5Database &db, QJsonArray &arr);

    void processAppOrder(QJsonObject &o);

    void processCallStaff(QJsonObject &o);

    void processMessageList(QJsonObject &o);

    void processCallReceipt(QJsonObject &o);

    QTcpSocket *fSocket;
};

#endif // C5WAITERSERVER_H
