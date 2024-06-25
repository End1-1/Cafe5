#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include "c5database.h"
#include "c5serverhandler.h"
#include "c5storedraftwriter.h"
#include <QObject>
#include <QJsonObject>

class QTcpSocket;

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer(QJsonObject &o, QTcpSocket *socket);

    void reply(QJsonObject &o);

    int cmd();

private:
    QJsonObject &fIn;

    QString fPeerAddress;

    bool checkPermission(int user, int permission, C5Database &db);

    void openActiveOrder(QJsonObject &jh, QJsonArray &jb, QJsonArray &jt, C5ServerHandler &srh);

    void saveOrder(QJsonObject &o, QJsonObject &jh, QJsonArray &ja, C5Database &db);

    void saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db);

    void processCloseOrder(QJsonObject &o, C5Database &db);

    int printTax(const QMap<QString, QVariant> &header, const QList<QMap<QString, QVariant> > &body, QString &err);

    bool printReceipt(QString &err, C5Database &db, bool isBill, bool alias);

    void remember(const QJsonObject &h);

    void processAppOrder(QJsonObject &o);

    void processCallStaff(QJsonObject &o);

    void processMessageList(QJsonObject &o);

    void processCallReceipt(QJsonObject &o);

    void processGetCostumerByCar(QJsonObject &o);

    void processRotateShift(QJsonObject &o);

    void processCheckDiscountByVisit(QJsonObject &o);

    void processPrintRemovedService(QJsonObject &o);

    void processReturnTaxReceipt(QJsonObject &o);

    void processTaxReport(QJsonObject &o);

    void writeCashDoc(C5StoreDraftWriter &dw, const QString &uuid, const QString id, QString &err, double amount, int staff,
                      int cashboxid, QDate dateCash);

    QTcpSocket *fSocket;
};

#endif // C5WAITERSERVER_H
