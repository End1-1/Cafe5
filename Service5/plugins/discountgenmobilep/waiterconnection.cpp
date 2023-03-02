#include "waiterconnection.h"
#include "ops.h"
#include "login.h"
#include "c5translator.h"
#include "logwriter.h"
#include <QDebug>
#include <QThread>


WaiterConnection::WaiterConnection(QHash<QString, QString> conn, SslSocket *s) :
    QObject(),
    fSocket(s),
    fDbConnectionSettings(conn),
    fUserId(0)
{

}

WaiterConnection::~WaiterConnection()
{
    qDebug() << "~WaiterConnection()";
}

void WaiterConnection::sendToAllClients(QByteArray data)
{
    emit broadcastForClients(data);
}

void WaiterConnection::handleData(RawMessage *m, const QByteArray &in)
{
    if (fSocket->fUuid != m->socket()->fUuid) {
        return;
    }
    RawMessage rm(m->socket());
    rm.clear(m->header());
    rm.setPosition(m->getPosition());
    quint32 op;
    rm.readUInt(op, in);
    rm.putUInt(op);

    Database dbw(fDbConnectionSettings["db_driver"]);
    if (dbw.open(fDbConnectionSettings["db_host"],
                fDbConnectionSettings["db_schema"],
                fDbConnectionSettings["db_user"],
                fDbConnectionSettings["db_password"]) == false) {
        LogWriter::write(LogWriterLevel::errors, rm.property("session").toString(), dbw.lastDbError());
        rm.putUByte(0);
        rm.putString(dbw.lastDbError());
        rm.writeToSocket();
        return;
    }

    switch (op) {
    case op_login:
        fUserId = loginPin(rm, dbw, in, fUserName);
        break;
    case op_login_passhash:
        fUserId = loginPasswordHash(rm, dbw, in, fUserName);
        break;
    case op_create_qr_discount:
        createQrDiscount(rm, dbw, in, this);
        break;
    case op_check_bonuses:
        checkBonuses(rm, dbw, in, this);
        break;
    default:
        rm.putUByte(0);
        rm.putString(QString("Not implemented: %1").arg(op));
        break;
    }
    rm.writeToSocket();
}

void WaiterConnection::removeSocket(SslSocket *s)
{
    if (fSocket->fUuid == s->fUuid) {
        emit removeMeFromConnectionList(fSocket->fUuid);
        qDebug() << "Waiter connection removed" << fSocket->fUuid;
        s->deleteLater();
        deleteLater();
    }
    if (fLockedTable != 0) {
        Database dbw;
        if (dbw.open(fDbConnectionSettings["db_host"],
                    fDbConnectionSettings["db_schema"],
                    fDbConnectionSettings["db_user"],
                    fDbConnectionSettings["db_password"])) {

        }
    }
}

void WaiterConnection::writeToSocket(QByteArray data)
{
    qDebug() << "Receive data for all connected clients";
    fSocket->write(data);
}
