#include "shopconnection.h"
#include "ops.h"
#include "login.h"
#include "translator.h"
#include "store.h"
#include "datadriver.h"
#include "logwriter.h"
#include "traiding.h"
#include <QDebug>
#include <QThread>

ShopConnection::ShopConnection(QHash<QString, QString> conn, SslSocket *s) :
    QObject(),
    fSocket(s),
    fDbConnectionSettings(conn),
    fUserId(0)
{

}

ShopConnection::~ShopConnection()
{
    qDebug() << "~WaiterConnection()";
}

void ShopConnection::sendToAllClients(QByteArray data)
{
    emit broadcastForClients(data);
}

void ShopConnection::handleData(RawMessage *m, const QByteArray &in)
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

    Database dbw(_DBDRIVER_);
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
    case op_check_qty:
        store_checkqty(rm, dbw, in);
        break;
    case op_data_currency_list:
        data_list(rm, dbw, "sq_data_currency_list");
        break;
    case op_data_currency_crossrate_list:
        data_list(rm, dbw, "sq_data_currency_crossrate_list");
        break;
    case op_get_goods_list:
        data_list(rm, dbw, "sq_data_goods_list");
        break;
    case op_goods_prices:
        data_list(rm, dbw, "sq_data_goods_prices");
        break;
    case op_create_empty_sale:
        create_empty_draft(rm, dbw, in, fUserId);
        break;
    case op_open_sale_draft_document:
        open_draft_header(rm, dbw, in);
        break;
    case op_show_drafts_sale_list:
        get_drafts_headers(rm, dbw, in, fUserId);
        break;
    case op_add_goods_to_draft:
        add_goods_to_draft(rm, dbw, in, fUserId);
        break;
    case op_open_sale_draft_body:
        open_draft_body(rm, dbw, in);
        break;
    default:
        rm.putUByte(0);
        rm.putString(QString("Not implemented: %1").arg(op));
        break;
    }
    rm.writeToSocket();
}

void ShopConnection::removeSocket(SslSocket *s)
{
    if (fSocket->fUuid == s->fUuid) {
        emit removeMeFromConnectionList(fSocket->fUuid);
        qDebug() << "Waiter connection removed" << fSocket->fUuid;
        s->deleteLater();
        deleteLater();
    }
}

void ShopConnection::writeToSocket(QByteArray data)
{
    qDebug() << "Receive data for all connected clients";
    fSocket->write(data);
}
