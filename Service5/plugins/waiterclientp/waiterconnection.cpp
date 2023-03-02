#include "waiterconnection.h"
#include "ops.h"
#include "halls.h"
#include "order.h"
#include "dishes.h"
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
        fUserId = loginUsernamePassword(rm, dbw, in, fUserName);
        break;
    case op_login_pin:
        fUserId = loginPin(rm, dbw, in, fUserName);
        break;
    case op_get_hall_list:
        loadHalls(rm, dbw, in);
        break;
    case op_get_table_list:
        loadTables(rm,  dbw, in);
        break;
    case op_get_dish_part1_list:
        loadPart1(rm, dbw, in);
        break;
    case op_get_dish_part2_list:
        loadPart2(rm, dbw, in);
        break;
    case op_get_dish_dish_list:
        loadDishes(rm, dbw, in);
        break;
    case op_dish_menu:
        loadDishMenu(rm, dbw, in);
        break;
    case op_login_passhash:
        fUserId = loginPasswordHash(rm, dbw, in, fUserName);
        break;

        /*ORDER COMMANDS */
    case op_open_table:
        if (fUserId == 0) {
            rm.putUByte(0);
            rm.putString(ntr("Unathorized", ntr_am));
        } else {
            openTable(rm, dbw, in, this);
        }
        break;
    case op_unlock_table:
        unlockTable(rm, dbw, in, fSocket->fUuid, this);
        break;
    case op_car_list:
        loadCarsModels(rm, dbw, in);
        break;
    case op_search_platenumber:
        searchPlateNumber(rm, dbw, in);
        break;
    case op_create_order_header:
        if (fUserId == 0) {
            rm.putUByte(0);
            rm.putString(tr_am("Unathorized"));
        } else {
            createHeaderOfOrder(rm, dbw, in, fUserId);
        }
        break;
    case op_set_car:
        setCar(rm, dbw, in, this);
        break;
    case op_get_car:
        getCar(rm, dbw, in);
        break;
    case op_open_order:
        openOrder(rm, dbw, in, fUserId);
        break;
    case op_add_dish_to_order:
        addDishToOrder(rm,dbw, in, fUserId);
        break;
    case op_remove_dish_from_order:
        removeDishFromOrder(rm, dbw, in, fUserId);
        break;
    case op_modify_order_dish:
        modifyDishOrder(rm, dbw, in, fUserId);
        break;
    case op_print_service:
        printService(rm, dbw, in, this);
        break;
    case op_get_dish_comment:
        getDishComment(rm, dbw, in);
        break;
    case op_print_bill:
        printBill(rm, dbw, in, fUserId);
        break;
    case op_get_menu_list:
        loadMenuNames(rm, dbw, in);
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
            forceUnlockTable(dbw, fSocket->fUuid, fProtocolVersion);
        }
    }
}

void WaiterConnection::writeToSocket(QByteArray data)
{
    qDebug() << "Receive data for all connected clients";
    fSocket->write(data);
}
