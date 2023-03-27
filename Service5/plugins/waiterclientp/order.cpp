#include "order.h"
#include "c5translator.h"
#include "networktable.h"
#include "ops.h"
#include "waiterconnection.h"
#include "messagelist.h"
#include "printtaxn.h"
#include "c5printing.h"
#include "printbill.h"
#include <QFont>
#include <QPrinter>

void openTable(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *wc)
{
    quint8 version;
    quint32 tableid;
    QString sessionid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readString(sessionid, in);
    QString orderid, owner;

    if (sessionid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(ntr("Empty session id", ntr_am));
        return;
    }

    //check locked
    db[":f_id"] = tableid;
    switch (version) {
    case version1:
        if (db.exec("select lock_host from h_table where id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("lock_host").isEmpty() == false) {
                if (db.string("lock_host") != sessionid) {
                    rm.putUByte(0);
                    rm.putString(ntr("Table already editing by other user", ntr_am));
                    return;
                }
            }
            db[":f_lockhost"] = sessionid;
            db[":f_id"] = tableid;
            db[":f_locktime"] = QDateTime::currentDateTime().toSecsSinceEpoch();
            if (db.exec("update h_table set lock_host=:f_lockhost, lock_time=:f_locktime where id=:f_id") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            db[":f_table"] = tableid;
            if (db.exec("select o.id, e.lname || ' ' || e.fname as staff "
                        "from o_order o "
                        "left join employes e on e.id=o.staff_id "
                        "where table_id=:f_table and o.state_id=1") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            if (db.next()) {
                orderid = db.string("id");
                owner = db.string("staff");
                db[":order_id"] = orderid;
                db.update("h_table", "id", tableid);
            }
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Wrong table id", ntr_am));
            return;
        }
        break;
    case version2:
        if (db.exec("select f_locksrc from h_tables where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("lock_host").isEmpty() == false) {
                if (db.string("f_locksrc") != sessionid) {
                    rm.putUByte(0);
                    rm.putString(ntr("Table already editing by other user", ntr_am));
                    return;
                }
            }
            db[":f_locksrc"] = sessionid;
            db[":f_id"] = tableid;
            db[":f_locktime"] = QDateTime::currentDateTime();
            if (db.exec("update h_tables set f_locksrc=:f_locksrc, f_locktime=:f_locktime where f_id=:f_id") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            db[":f_table"] = tableid;
            if (db.exec("select o.f_id, concat_ws(' ', u.f_last, u.f_first) as staff "
                        "from o_header o "
                        "left join s_user u on u.f_id=o.f_staff "
                        "where o.f_table=:f_table and o.f_state=1") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            if (db.next()) {
                orderid = db.string("id");
                owner = db.string("staff");
            }
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Wrong table id", ntr_am));
            return;
        }
        break;
    case version3:
        if (db.exec("select f_lockhost from r_table where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("f_lockhost").isEmpty() == false) {
                if (db.string("f_lockhost") != sessionid) {
                    rm.putUByte(0);
                    rm.putString(ntr("Table already editing by other user", ntr_am));
                    return;
                }
            }
            db[":f_lockhost"] = sessionid;
            db[":f_id"] = tableid;
            if (db.exec("update r_table set f_lockhost=:f_lockhost, f_locktime=unix_timestamp(now()) where f_id=:f_id") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            db[":f_table"] = tableid;
            if (db.exec("select f_id from o_header where f_table=:f_table and f_state=1") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            if (db.next()) {
                orderid = db.string("f_id");
                db[":f_order"] = orderid.toInt();
                db.update("r_table", "f_id", tableid);
            }
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Wrong table id", ntr_am));
            return;
        }
        break;
    }

    rm.putUByte(1);
    rm.putString(orderid);
    rm.putString(owner);
    wc->fLockedTable = tableid;
    wc->fProtocolVersion = version;
}

void unlockTable(RawMessage &rm, Database &db, const QByteArray &in, const QString &session, WaiterConnection *wc)
{
    quint8 version;
    quint32 tableid;
    QString sessionid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readString(sessionid, in);

    if (sessionid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(ntr("Empty session id", ntr_am));
        return;
    }

    db[":f_id"] = tableid;
    switch (version) {
    case version1:
        if (db.exec("select lock_host from h_table where id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("lock_host").isEmpty() == false) {
                if (db.string("lock_host").toLower() != sessionid.toLower()) {
                    rm.putUByte(0);
                    rm.putString(ntr("You cannot unlock host thats not locked by you", ntr_am));
                    return;
                }
            }
            db[":f_lockhost"] = sessionid;
            db.exec("update h_table set lock_host='', lock_time=0 where lock_host=:f_lockhost");
        } else {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        break;
    case version2:
        if (db.exec("select f_locksrc from h_tables where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("f_locksrc").isEmpty() == false) {
                if (db.string("f_locksrc").toLower() != sessionid.toLower()) {
                    rm.putUByte(0);
                    rm.putString(ntr("You cannot unlock host thats not locked by you", ntr_am));
                    return;
                }
            }
            db[":f_locksrc"] = sessionid;
            db.exec("update h_tables set f_locksrc='', f_locktime=null where f_locksrc=:f_locksrc");
        } else {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        break;
    case version3:
        if (db.exec("select f_lockhost from r_table where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("f_lockhost").isEmpty() == false) {
                if (db.string("f_lockhost").toLower() != sessionid.toLower()) {
                    rm.putUByte(0);
                    rm.putString(ntr("You cannot unlock host thats not locked by you", ntr_am));
                    return;
                }
            }
            db[":f_lockhost"] = sessionid;
            db.exec("update r_table set f_lockhost='', f_locktime=0 where f_lockhost=:f_lockhost");
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return;
        }
        break;
    }

    rm.putUByte(1);

    RawMessage r(nullptr);
    r.setHeader(0, 0, MessageList::dll_plugin);
    r.putUInt(op_update_tables);
    r.putUByte(1);
    r.putUInt(tableid);
    r.putString("");
    wc->sendToAllClients(r.data());
}

void createHeaderOfOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    quint32 tableid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);

    quint8 neworder = 1;
    QString orderid;
    db[":f_table"] = tableid;
    switch (version) {
    case 1:
        if (db.exec("select id from o_order where state_id=1 and table_id=:f_table") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            neworder = 0;
            orderid = db.string("id");
        } else {
            db.exec("select gen_id(GEN_O_ORDER_ID, 1) as new_id from rdb$database");
            db.next();
            int new_id = db.integer("new_id");
            db[":f_id"] = tableid;
            if (db.exec("select * from h_hall where id in (select hall_Id from h_table where id=:f_id)") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            if (!db.next()) {
                rm.putUByte(0);
                rm.putString(tr_am("Invalid table id"));
                return;
            }
            orderid = QString("%1%2").arg(db.string("order_prefix"), QString::number(new_id));
            //int branch = db.integer("f_branch");
            double service_value = db.doubleValue("service_value");

            db[":id"] = orderid;
            db[":state_id"] = 1;
            db[":table_id"] = tableid;
            db[":date_open"] = QDateTime::currentDateTime();
            db[":staff_id"] = userid;
            db[":amount"] = 0;
            db[":print_qty"] = 0;
            db[":amount_inc"] = 0;
            db[":amount_inc_value"] = service_value;
            db[":amount_dec"] = 0;
            db[":amount_dec_value"] = 0;
            if (db.insert("o_order") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
        }
        break;
    case 2:
        createHeaderOfOrderV2(rm, db, in, userid, tableid);
        return;
    case 3:

        break;
    default:
        break;
    }

    rm.putUByte(1);
    rm.putUByte(neworder);
    rm.putString(orderid);
}

void setCar(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w)
{
    quint8 version;
    quint32 tableid, carid, customerid;
    QString plate, customername, info;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readUInt(carid, in);
    rm.readString(plate, in);
    rm.readUInt(customerid, in);
    rm.readString(customername, in);
    rm.readString(info, in);

    QString orderid;
    int hallid;
    double servicevalue;

    switch (version) {
    case 1:
        break;
    case 2:
        break;
    case 3: {
        qDebug() << plate << customerid << customername << info;
        if (customerid == 0) {
            db[":f_name"] = customername;
            db[":f_info"] = info;
            int ci;
            db.insert("o_debt_holder", ci);
            customerid = ci;
        } else {
            db[":f_name"] = customername;
            db[":f_info"] = info;
            db.update("o_debt_holder", "f_id", customerid);
        }
        db[":f_id"] = tableid;
        db.exec("select f_hall from r_table where f_id=:f_id");
        db.next();
        hallid = db.integer("f_hall");
        db[":f_id"] = hallid;
        db.exec("select * from r_hall where f_id=:f_id");
        db.next();
        servicevalue = db.doubleValue("f_servicevalue");
        int branch = db.integer("f_branch");
        db[":f_table"] = tableid;
        if (db.exec("select * from o_header where f_table=:f_table and f_state=1") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            orderid = db.string("f_id");
            db[":f_order"] = orderid;
            db[":f_model"] = carid;
            db[":f_govnumber"] = plate;
            db[":f_costumer"] = customerid;
            db.exec("update o_car set f_model=:f_model, f_govnumber=:f_govnumber, f_costumer=:f_costumer where f_order=:f_order");
        } else {
            db[":f_state"] = 1;
            db[":f_branch"] = branch;
            db[":f_table"] = tableid;
            db[":f_staff"] = w->fUserId;
            db[":f_dateOpen"] = QDateTime::currentDateTime();
            db[":f_dateCash"] = QDate::currentDate();
            db[":f_tax"] = 0;
            db[":f_paymentMode"] = 1;
            db[":f_hall"] = hallid;
            db[":f_servicevalue"] = servicevalue;
            int oi;
            db.insert("o_header", oi);
            db[":f_id"] = tableid;
            db[":f_order"] = oi;
            db.exec("update r_table set f_order=:f_order where f_id=:f_id");
            orderid = QString::number(oi);
            db[":f_order"] = orderid;
            db[":f_model"] = carid;
            db[":f_govnumber"] = plate;
            db[":f_costumer"] = customerid;
            db.insert("o_car");
        }
        break;
    }
    default:
        break;
    }
    rm.putUByte(1);
    rm.putString(orderid);
    rm.putUInt(customerid);
    rm.putString(customername);
    rm.putString(info);

    RawMessage r(nullptr);
    r.setHeader(0, 0, MessageList::dll_plugin);
    r.putUInt(op_update_tables);
    r.putUByte(1);
    r.putUInt(tableid);
    r.putString(orderid);
    w->sendToAllClients(r.data());
}

void openOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    QString orderid;
    rm.readUByte(version, in);
    rm.readString(orderid, in);
    if (orderid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr_am("Order id is empty"));
        return;
    }

    db[":f_header"] = orderid;
    switch (version) {
    case version1:{
        NetworkTable ntdishes(rm, db);
        db[":f_header"] = orderid;
        ntdishes.execSQL("select id, dish_id, qty, printed_qty, price,"
                        "price_inc, price_dec, store_id, print1, print2, comments "
                        "from o_dishes "
                        "where order_id=:f_header and state_id=1 ");
        break;
    }
    case version2: {
        NetworkTable ntdishes(rm, db);
        db[":f_header"] = orderid;
        ntdishes.execSQL("select f_id, f_dish, f_qty1, f_qty2, f_price,"
                        "f_service, f_discount, f_store, f_print1, f_print2, f_comment "
                        "from o_body "
                        "where f_header=:f_header and f_state=1 ");
        break;
    }
    case version3: {
        NetworkTable ntdishes(rm, db);
        ntdishes.execSQL("select f_id, f_dish, f_qty, f_qtyprint, f_price,"
                        "f_svcvalue, f_dctValue, f_store, f_print1, f_print2, f_comment "
                        "from o_dish "
                        "where f_header=:f_header and f_state=1 ");
        break;
    }
    }

    rm.putUByte(1);
}

void getCar(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    QString orderid;
    quint32 carid = 0, customerid = 0;
    QString plate, customername, info;
    rm.readUByte(version, in);
    rm.readString(orderid, in);

    switch (version) {
    case 1:
         break;
    case 2:
        break;
    case 3:
        db[":f_order"] = orderid;
        db.exec("select f_model, f_costumer, f_govnumber from o_car where f_order=:f_order");
        if (db.next()) {
            carid = db.integer("f_model");
            customerid = db.integer("f_costumer");
            plate = db.string("f_govnumber");
        }
        if (customerid > 0) {
            db[":f_id"] = customerid;
            db.exec("select f_name, f_info from o_debt_holder where f_id=:f_id");
            if (db.next()) {
                customername = db.string("f_name");
                info = db.string("f_info");
            }
        }
        break;
    }

    rm.putUByte(1);
    rm.putUInt(carid);
    rm.putString(plate);
    rm.putUInt(customerid);
    rm.putString(customername);
    rm.putString(info);
}

void forceUnlockTable(Database &db, const QString &sessionid, int version)
{
    switch (version) {
    case 1:
        break;
    case 2:

        break;
    case 3:
        db[":f_lockhost"] = sessionid;
        db.exec("update r_table set f_lockhost='', f_locktime=0 where f_lockhost=:f_lockhost");
        break;
    }
}

void addDishToOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    QString orderid;
    quint32 dishid, storeid;
    QString print1, print2, comment;
    double price;
    rm.readUByte(version, in);
    rm.readString(orderid, in);
    rm.readUInt(dishid, in);
    rm.readDouble(price, in);
    rm.readUInt(storeid, in);
    rm.readString(print1, in);
    rm.readString(print2, in);
    rm.readString(comment, in);

    if (orderid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr_am("Empty order id"));
        return;
    }

    double svcvalue = 0, svcamount = 0, dctvalue = 0 , dctamount = 0, total = price;
    switch (version) {
    case 1: {
        db[":f_id"] = orderid;
        if (db.exec("select * from O_ORDER where id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Empty order id"));
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
        if (db.integer("print_qty") > 0) {
            rm.putUByte(0);
            rm.putString(tr_am("This order is not editable"));
            return;
        }
        svcvalue = db.doubleValue("amount_inc_value");
        svcamount = price * (svcamount / 100);
        int id = db.genFBID("GEN_O_DISH_ID");
        db[":id"] = id;
        db[":order_id"] = orderid;
        db[":state_id"] = 1;
        db[":dish_id"] = dishid;
        db[":qty"] = 1;
        db[":printed_qty"] = 0;
        db[":price"] = price;
        db[":last_user"] = userid;
        db[":store_id"] = storeid;
        db[":print1"] = print1;
        db[":print2"] = print2;
        db[":comments"] = comment;
        db[":payment_mod"] = 1;
        db[":price_inc"] = svcvalue;
        db[":price_dec"] = dctvalue;
        db[":remind"] = 0;
        db[":f_storestate"] = 0;
        db[":f_removereason"] = "";
        if (db.insert("o_dishes") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!countOrder1(rm, db, orderid)){
            return;
        }
        rm.putUByte(1);
        rm.putString(QString::number(id));
        rm.putUInt(dishid);
        rm.putDouble(1);
        rm.putDouble(0);
        rm.putDouble(price);
        rm.putDouble(svcvalue);
        rm.putDouble(dctvalue);
        rm.putUInt(storeid);
        rm.putString(print1);
        rm.putString(print2);
        rm.putString(comment);
        break;
    }
    case 2: {
        db[":f_id"] = orderid;
        if (db.exec("select * from o_header where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Empty order id"));
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
//        if (db.integer("f_print") > 0) {
//            rm.putUByte(0);
//            rm.putString(tr_am("This order is not editable"));
//            return;
//        }
        svcvalue = db.doubleValue("f_servicefactor");
        svcamount = price * (svcamount / 100);
        QString id = db.uuid();
        db[":f_id"] = id;
        db[":f_header"] = orderid;
        db[":f_state"] = 1;
        db[":f_dish"] = dishid;
        db[":f_qty1"] = 1;
        db[":f_qty2"] = 0;
        db[":f_price"] = price;
        db[":f_total"] = price;
        db[":f_appenduser"] = userid;
        db[":f_store"] = storeid;
        db[":f_print1"] = print1;
        db[":f_print2"] = print2;
        db[":f_comment"] = comment;
        db[":f_canservice"] = 1;
        db[":f_candiscount"] = 1;
        db[":f_service"] = svcvalue;
        db[":f_discount"] = dctvalue;
        db[":f_remind"] = 0;
        db[":f_removereason"] = "";
        if (db.insert("o_body") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!countOrder2(rm, db, orderid)){
            return;
        }
        rm.putUByte(1);
        rm.putString(id);
        rm.putUInt(dishid);
        rm.putDouble(1);
        rm.putDouble(0);
        rm.putDouble(price);
        rm.putDouble(svcvalue);
        rm.putDouble(dctvalue);
        rm.putUInt(storeid);
        rm.putString(print1);
        rm.putString(print2);
        rm.putString(comment);
        break;
    }
    case 3: {
        db[":f_id"] = orderid;
        if (db.exec("select * from o_header where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Empty order id"));
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
        svcvalue = db.doubleValue("f_servicevalue");
        svcamount = price * (svcamount / 100);

        int hall = db.integer("f_hall");
        db[":f_id"] = hall;
        db.exec("select * from r_hall where f_id=:f_id");
        db.next();
        int branch = db.integer("f_branch");
        db[":f_branch"] = branch;
        db[":f_store"] = storeid;
        db.exec("select f_alias from r_branch_storemap where f_branch=:f_branch and f_store=:f_store");
        if (db.next() == false) {
            rm.putUByte(0);
            rm.putString(tr_am("No store alias exists for selected store"));
            return;
        }
        storeid = db.integer("f_alias");

        db[":f_header"] = orderid.toInt();
        db[":f_state"] = 1;
        db[":f_dish"] = dishid;
        db[":f_qty"] = 1;
        db[":f_qtyprint"] = 0;
        db[":f_price"] = price;
        db[":f_svcamount"] = svcamount;
        db[":f_svcvalue"] = svcvalue;
        db[":f_dctvalue"] = dctvalue;
        db[":f_dctamount"] = dctamount;
        db[":f_total"] = total;
        db[":f_totalusd"] = total;
        db[":f_print1"] = print1;
        db[":f_print2"] = print2;
        db[":f_store"] = storeid;
        db[":f_comment"] = "";
        db[":f_staff"] = userid;
        db[":f_complex"] = 0;
        db[":f_complexid"] = 0;
        db[":f_canceluser"] = 0;
        int id = 0;
        if (db.insert("o_dish", id) == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }

        db[":f_header"] = orderid.toInt();
        db[":f_state"] = 1;
        db.exec("update o_header set f_total=(select sum(f_total) from o_dish where f_header=:f_header and f_state=:f_state) where f_id=:f_header");


        rm.putUByte(1);
        rm.putString(QString::number(id));
        rm.putUInt(dishid);
        rm.putDouble(1);
        rm.putDouble(0);
        rm.putDouble(price);
        rm.putDouble(svcvalue);
        rm.putDouble(dctvalue);
        rm.putUInt(storeid);
        rm.putString(print1);
        rm.putString(print2);
        rm.putString(comment);
        break;
    }
    }
}

void removeDishFromOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    QString recid, orderid;
    rm.readUByte(version, in);
    rm.readString(recid, in);

    switch (version) {
    case 1:
        db[":f_id"] = recid.toInt();
        if (db.exec("select * from o_dishes where id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Record not exists"));
            return;
        }
        if (db.doubleValue("printed_qty") > 0) {
            //check right for remove from order
            rm.putUByte(0);
            rm.putString(tr_am("Need raise your rights"));
            return;
        }
        db[":f_id"] = recid.toInt();
        db[":f_state"] = 0;
        db.exec("update o_dishes set state_id=:f_state where id=:f_id");
        break;
    case 2:
        db[":f_id"] = recid;
        if (db.exec("select * from o_body where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Record not exists"));
            return;
        }
        if (db.doubleValue("f_qty2") > 0.001) {
            //check right for remove from order
            rm.putUByte(0);
            rm.putString(tr_am("Need raise your rights"));
            return;
        }
        db[":f_id"] = recid;
        db[":f_state"] = 0;
        db[":f_removeuser"] = userid;
        if (db.exec("update o_body set f_state=:f_state, f_removeuser=:f_removeuser, f_removetime=current_timestamp() where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
        }
        break;
    case 3:
        db[":f_id"] = recid.toInt();
        if (db.exec("select * from o_dish where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(tr_am("Record not exists"));
            return;
        }
        if (db.doubleValue("f_qtyprint") > 0) {
            //check right for remove from order
            rm.putUByte(0);
            rm.putString(tr_am("Need raise your rights"));
            return;
        }
        orderid = db.string("f_header");
        db[":f_id"] = recid.toInt();
        db[":f_state"] = 2;
        db.exec("update o_dish set f_state=:f_state where f_id=:f_id");

        db[":f_header"] = orderid.toInt();
        db[":f_state"] = 1;
        db.exec("update o_header set f_total=(select sum(f_total) from o_dish where f_header=:f_header and f_state=:f_state) where f_id=:f_header");
        break;
    }
    rm.putUByte(1);
    rm.putString(recid);
}

void modifyDishOrder(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    QString recid, comment, orderid;
    double qty;

    rm.readUByte(version, in);
    rm.readString(recid, in);
    rm.readDouble(qty, in);
    rm.readString(comment, in);

    double price = 0, total = 0;
    double disc = 0;

    switch (version) {
    case 1:
        db[":id"] = recid;
        db.exec("select * from o_dishes where id=:id");
        if (db.next()) {
            price = db.doubleValue("price");
            total = qty * price;
            orderid = db.string("order_id");
        } else {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid record id"));
            return;
        }
        db[":id"] = orderid;
        if (db.exec("select * from o_order where id=:id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next() == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
        if (db.integer("print_qty") > 0) {
            rm.putUByte(0);
            rm.putString(tr_am("This order is not editable"));
            return;
        }
        db[":qty"] = qty;
        db[":comments"] = comment;
        if (db.update("o_dishes", "id", recid) == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!countOrder1(rm, db, orderid)) {
            return;
        }
        break;
    case 2:
        db[":f_id"] = recid;
        db.exec("select * from o_body where f_id=:f_id");
        if (db.next()) {
            price = db.doubleValue("f_price");
            total = qty * price;
            orderid = db.string("f_header");
        } else {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid record id"));
            return;
        }
        db[":f_id"] = orderid;
        if (db.exec("select * from o_header where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next() == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
//        if (db.integer("f_qty2") > 0) {
//            rm.putUByte(0);
//            rm.putString(tr_am("This order is not editable"));
//            return;
//        }
        db[":f_qty1"] = qty;
        db[":f_total"] = total;
        db[":f_comment"] = comment;
        if (db.update("o_body", "f_id", recid) == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!countOrder2(rm, db, orderid)) {
            return;
        }
        break;
    case 3:
        db[":f_id"] = recid;
        db.exec("select * from o_dish where f_id=:f_id");
        if (db.next()) {
            price = db.doubleValue("f_price");
            total = qty * price;
        } else {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid record id"));
            return;
        }
        db[":f_qty"] = qty;
        db[":f_total"] = total;
        db[":f_totalusd"] = total;
        db[":f_comment"] = comment;
        if (db.update("o_dish", "f_id", recid) == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }

        db[":f_header"] = orderid.toInt();
        db[":f_state"] = 1;
        db.exec("update o_header set f_total=(select sum(f_total) from o_dish where f_header=:f_header and f_state=:f_state) where f_id=:f_header");
        break;
    }
    rm.putUByte(1);
    rm.putString(recid);
    rm.putDouble(qty);
    rm.putString(comment);
}

void printService(RawMessage &rm, Database &db, const QByteArray &in, WaiterConnection *w)
{
    quint8 version;
    QString orderid;

    rm.readUByte(version, in);
    rm.readString(orderid, in);

    switch (version) {
    case 1: {
        QMap<QString, QVariant> order;
        db[":id"] = orderid;
        if (db.exec("select o.id, o.state_id as state, o.table_id, t.name as tablename, h.name as hallname, "
                    "o.staff_id "
                    "from o_order o "
                    "left join h_table t on t.id=o.table_id "
                    "left join h_hall h on h.id=t.hall_id "
                    "where o.id=:id ") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next() == false) {
            rm.putUByte(0);
            rm.putString(tr_am("Invalid order id"));
            return;
        }
        db.rowToMap(order);
        order["current_staff"] = w->fUserName;

        db[":f_header"] = orderid;
        if (db.exec("select od.*,  od.comments as comment, d.name as dishname, st.name as storename "
                    "from o_dishes od "
                    "left join me_dishes d on d.id=od.dish_id "
                    "left join st_storages st on st.id=od.store_id "
                    "where od.order_id=:f_header and od.state_id=1 and od.printed_qty<od.qty") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        QList<QMap<QString, QVariant> > dishes;
        while (db.next()) {
            QMap<QString, QVariant> d;
            for (int i = 0; i < db.columnCount(); i++) {
                d[db.columnName(i)] = db.value(i);
            }
            dishes.append(d);
        }

        if (dishes.empty()) {
            rm.putUByte(0);
            rm.putString(tr_am("Nothing to print"));
            return;
        }

        QMap<QString, QList<int> > printers;
        for (int i = 0; i < dishes.count(); i++) {
            QMap<QString, QVariant> &d = dishes[i];
            if (d["print1"].toString().contains("mobile", Qt::CaseInsensitive)) {
                QStringList rd = d["print1"].toString().split(":", Qt::SkipEmptyParts);
                QString reminder = rd.at(1);
                db[":record_id"] = d["id"];
                db[":state_Id"] = 0;
                db[":date_register"] = QDateTime::currentDateTime();
                db[":staff_id"] = d["last_user"];
                db[":table_id"] = order["table_id"];
                db[":dish_id"] = d["dish_id"];
                db[":qty"] = d["qty"];
                db[":reminder_id"] = reminder.toInt();
                db.insert("o_dishes_reminder");
            } else if (d["print1"].toString().contains("printer", Qt::CaseInsensitive)) {
                QStringList rd = d["print1"].toString().split(":", Qt::SkipEmptyParts);
                QString printer = rd.at(1);
                printers[printer + ":" + d["storename"].toString() + ":1"].append(i);
            }
            if (d["print2"].toString() != d["print1"].toString()) {
                if (d["print2"].toString().contains("mobile", Qt::CaseInsensitive)) {
                    QStringList rd = d["print1"].toString().split(":", Qt::SkipEmptyParts);
                    QString reminder = rd.at(1);
                    db[":record_id"] = d["id"];
                    db[":state_Id"] = 0;
                    db[":date_register"] = QDateTime::currentDateTime();
                    db[":staff_id"] = d["last_user"];
                    db[":table_id"] = order["table_id"];
                    db[":dish_id"] = d["dish_id"];
                    db[":qty"] = d["qty"];
                    db[":reminder_id"] = reminder.toInt();
                    db.insert("o_dishes_reminder");
                } else if (d["print2"].toString().contains("printer", Qt::CaseInsensitive)) {
                    QStringList rd = d["print2"].toString().split(":", Qt::SkipEmptyParts);
                    QString printer = rd.at(1);
                    printers[printer + ":" + d["storename"].toString() + ":2"].append(i);
                }
            }
        }

        if (!printServiceOnPrinter(order, printers, dishes)) {
            return;
        }

        db[":f_header"] = orderid;
        if (db.exec("update o_dishes set printed_qty=qty where order_id=:f_header and state_id=1") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        break;
    }
    case 2:
    {
            QMap<QString, QVariant> order;
            db[":f_id"] = orderid;
            if (db.exec("select o.f_id, concat(o.f_prefix, o.f_hallid) as id, o.f_state as state, o.f_table, t.f_name as tablename, h.f_name as hallname, "
                        "o.f_currentstaff "
                        "from o_header o "
                        "left join h_tables t on t.f_id=o.f_table "
                        "left join h_halls h on h.f_id=t.f_hall "
                        "where o.f_id=:f_id ") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            if (db.next() == false) {
                rm.putUByte(0);
                rm.putString(tr_am("Invalid order id"));
                return;
            }
            db.rowToMap(order);
            order["current_staff"] = w->fUserName;

            db[":f_header"] = orderid;
            if (db.exec("select od.*, od.f_comment as comment, od.f_qty1 as qty, d.f_name as dishname, st.f_name as storename "
                        "from o_body od "
                        "left join d_dish d on d.f_id=od.f_dish "
                        "left join c_storages st on st.f_id=od.f_store "
                        "where od.f_header=:f_header and od.f_state=1 and od.f_qty2<od.f_qty1") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            QList<QMap<QString, QVariant> > dishes;
            while (db.next()) {
                QMap<QString, QVariant> d;
                for (int i = 0; i < db.columnCount(); i++) {
                    d[db.columnName(i)] = db.value(i);
                }
                dishes.append(d);
            }

            if (dishes.empty()) {
                rm.putUByte(0);
                rm.putString(tr_am("Nothing to print"));
                return;
            }

            QMap<QString, QList<int> > printers;
            for (int i = 0; i < dishes.count(); i++) {
                QMap<QString, QVariant> &d = dishes[i];
                if (d["f_print1"].toString().contains("mobile", Qt::CaseInsensitive)) {
//                    QStringList rd = d["f_print1"].toString().split(":", Qt::SkipEmptyParts);
//                    QString reminder = rd.at(1);
//                    db[":record_id"] = d["id"];
//                    db[":state_Id"] = 0;
//                    db[":date_register"] = QDateTime::currentDateTime();
//                    db[":staff_id"] = d["last_user"];
//                    db[":table_id"] = order["table_id"];
//                    db[":dish_id"] = d["dish_id"];
//                    db[":qty"] = d["qty"];
//                    db[":reminder_id"] = reminder.toInt();
//                    db.insert("o_dishes_reminder");
                } else if (d["f_print1"].toString().contains("printer", Qt::CaseInsensitive)) {
                    QStringList rd = d["f_print1"].toString().split(":", Qt::SkipEmptyParts);
                    QString printer = rd.at(1);
                    printers[printer + ":" + d["storename"].toString() + ":1"].append(i);
                } else if (!d["f_print1"].toString().isEmpty()) {
                    printers[d["f_print1"].toString() + ":" + d["storename"].toString() + ":1"].append(i);
                }
                if (d["f_print2"].toString() != d["f_print1"].toString()) {
                    if (d["f_print2"].toString().contains("mobile", Qt::CaseInsensitive)) {
//                        QStringList rd = d["print1"].toString().split(":", Qt::SkipEmptyParts);
//                        QString reminder = rd.at(1);
//                        db[":record_id"] = d["id"];
//                        db[":state_Id"] = 0;
//                        db[":date_register"] = QDateTime::currentDateTime();
//                        db[":staff_id"] = d["last_user"];
//                        db[":table_id"] = order["table_id"];
//                        db[":dish_id"] = d["dish_id"];
//                        db[":qty"] = d["qty"];
//                        db[":reminder_id"] = reminder.toInt();
//                        db.insert("o_dishes_reminder");
                    } else if (d["f_print2"].toString().contains("printer", Qt::CaseInsensitive)) {
                        QStringList rd = d["print2"].toString().split(":", Qt::SkipEmptyParts);
                        QString printer = rd.at(1);
                        printers[printer + ":" + d["storename"].toString() + ":2"].append(i);
                    } else if (!d["f_print2"].toString().isEmpty()) {
                        printers[d["f_print2"].toString() + ":" + d["storename"].toString() + ":2"].append(i);
                    }
                }
            }

            if (!printServiceOnPrinter(order, printers, dishes)) {
                return;
            }

            db[":f_header"] = orderid;
            db[":f_printuser"] = w->fUserId;
            if (db.exec("update o_body set f_qty2=f_qty1, f_printtime=current_timestamp, f_printuser=:f_printuser where f_header=:f_header and f_state=1") == false) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return;
            }
            break;
        }
    case 3:
        db[":f_header"] = orderid.toInt();
        if (db.exec("update o_dish set f_qtyprint=f_qty where f_header=:f_header and f_state=1") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        break;
    }
    rm.putUByte(1);

    RawMessage r(nullptr);
    r.setHeader(0, 0, MessageList::dll_plugin);
    r.putUInt(op_update_tables);
    r.putUByte(1);
    r.putUInt(0);
    r.putString(orderid);
    w->sendToAllClients(r.data());
}

bool countOrder1(RawMessage &rm, Database &db, const QString &orderid)
{
    db[":order_id"] = orderid;
    db[":state_id"] = 1;
    if (db.exec("select * from o_dishes where order_id=:order_id and state_id=:state_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    double total = 0, total_service = 0, total_disc = 0;
    while (db.next()) {
        double t = db.doubleValue("qty") * db.doubleValue("price");
        double ts = t * db.doubleValue("price_inc");
        t += ts;
        double td = t * db.doubleValue("price_dec");
        t -= td;
        total += t;
        total_service += ts;
        total_disc += td;
    }
    db[":amount"] = total;
    db[":amount_inc"] = total_service;
    db[":amount_dec"] = total_disc;
    if (db.update("o_order", "id", orderid) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    return true;
}

void printBill(RawMessage &rm, Database &db, const QByteArray &in, int userid)
{
    quint8 version;
    QString orderid;

    rm.readUByte(version, in);
    rm.readString(orderid, in);
    switch (version) {
    case 1:
        printBill1(rm, db, orderid);
        break;
    case 2:
        printBill2(rm, db, orderid);
        break;
    case 3:
        rm.putUByte(0);
        rm.putString("Print bill not implemented");
        return;
    }

    rm.putUByte(1);
}

bool printBill1(RawMessage &rm, Database &db, const QString &orderid)
{
    int bs = 18;
    QFont font("Arial LatArm Unicode");
    font.setPointSize(bs);
    C5Printing p;
    p.setSceneParams(650, 2600, QPrinter::Portrait);
    p.setFont(font);

    /* Fetch data */
    QMap<QString, QVariant> orderinfo;
    QMap<QString, QVariant> taxinfo;
    QList<QMap<QString, QVariant> > dishesinfo;

    db[":order_id"] = orderid;
    if (db.exec("select h.name as hallname, t.name as tablename, h.service_value, h.settings, "
                "e.lname || ' ' || e.fname as staffname, h.printer, o.print_qty, "
                "o.amount, o.amount_card, o.amount_inc, o.amount_inc_value, o.amount_dec, o.amount_dec_value "
                "from o_order o "
                "left join employes e on e.id=o.staff_id "
                "left join h_table t on t.id=o.table_id "
                "left join h_hall h on h.id=t.hall_id "
                "where o.id=:order_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(tr_am("Invalid order id"));
        return false;
    }
    db.rowToMap(orderinfo);

    db[":order_id"] = orderid;
    if (db.exec("select mt.adgcode, md.name as dishname, "
                "md.payment_mod, od.price, sum(od.printed_qty) as qty, sum(od.printed_qty*od.price)as total "
                "from o_dishes od "
                "left join me_dishes md on md.id=od.dish_id "
                "left join me_types mt on mt.id=md.type_id "
                "where od.order_id=:order_id and od.state_id=1 "
                "group by 1,2,3,4 ") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    while (db.next()) {
        QMap<QString, QVariant> d;
        db.rowToMap(d);
        dishesinfo.append(d);
    }

    /* checking */

    if (orderinfo["taxprint"].toInt() == 0) {
        if (!printTax1(rm, db, orderid)) {
            return false;
        }
    }
    db[":fid"] = orderid;
    if (!db.exec("select * from o_tax where fid=:fid")) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    if (!db.next()) {
        rm.putUByte(0);
        rm.putString(tr_am("Missing fiscal info"));
        return false;
    }
    db.rowToMap(taxinfo);

    if (orderinfo["print_qty"].toInt() > 0) {
        rm.putUByte(0);
        rm.putString(tr_am("Please, call to cashier"));
        return false;
    }

    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.setFontBold(true);
    p.ctext(QString("%1 %2").arg(tr_am("Receipt #"), orderid));
    p.br();
    p.setFontBold(false);

    QString firm, hvhh, fiscal, number, sn, address, devnum, time;
    PrintTaxN ptn;
    ptn.parseResponse(taxinfo["json"].toString(), firm, hvhh, fiscal, number, sn, address, devnum, time);
    if (number.toInt() > 0) {
        p.ltext(firm, 0);
        p.br();
        p.ltext(address, 0);
        p.br();
        p.ltext(tr_am("Department"), 0);
        p.rtext(taxinfo["f_dept"].toString());
        p.br();
        p.ltext(tr_am("Tax number"), 0);
        p.rtext(hvhh);
        p.br();
        p.ltext(tr_am("Device number"), 0);
        p.rtext(devnum);
        p.br();
        p.ltext(tr_am("Serial"), 0);
        p.rtext(sn);
        p.br();
        p.ltext(tr_am("Fiscal"), 0);
        p.rtext(fiscal);
        p.br();
        p.ltext(tr_am("Receipt number"), 0);
        p.rtext(number);
        p.br();
        p.ltext(tr_am("Date"), 0);
        p.rtext(time);
        p.br();
        p.ltext(tr_am("(F)"), 0);
        p.br();
    }
    p.br(1);
    p.ltext(tr_am("Table"), 0);
    p.rtext(QString("%1/%2").arg(orderinfo["hallname"].toString(), orderinfo["tablename"].toString()));
    p.br();
    p.ltext(tr_am("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(tr_am("Staff"), 0);
    p.rtext(orderinfo["staffname"].toString());
    p.br();
    p.br(2);
    p.line();
    p.br(2);
    p.ctext(tr_am("Class | Name | Qty | Price | Total"));
    p.br();
    p.br(4);
    p.line();
    p.br(1);

    bool noservice = false;
    for (int i = 0; i < dishesinfo.count(); i++) {
        const QMap<QString, QVariant> &m = dishesinfo.at(i);
        QString name = QString("%1: %2").arg(tr_am("Class"), m["f_adgcode"].toString());
        name += m["dishname"].toString();
        if (m["payment_mod"].toInt() != 1) {
            noservice = true;
            name += "*";
        }
        p.ltext(name, 0);
        p.br();
        p.rtext(QString("%1 x %2 = %3").arg(float_str(m["qty"].toDouble(), 2),
                float_str(m["price"].toDouble(), 2), float_str(m["total"].toDouble(), 2)));
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }
    p.br();
    p.ltext(tr_am("Total"), 0);
    p.rtext(float_str(orderinfo["amount"].toDouble(), 2));
    p.br();
    p.br(1);
    if (orderinfo["amount_inc"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(tr_am("Service"), float_str(orderinfo["amount_inc_value"].toDouble() * 100, 2)), 0);
        p.rtext(float_str(orderinfo["amount_inc"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    if (orderinfo["amount_dec"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(tr_am("Discount"), float_str(orderinfo["amount_dec_value"].toDouble() * 100, 2)), 0);
        p.rtext(float_str(orderinfo["amount_dec"].toDouble(), 2));
        p.br();
        p.br(1);
    }

    p.setFontSize(bs + 4);
    p.setFontBold(true);
    p.ltext(tr_am("Need to pay"), 0);
    p.setFontSize(bs + 8);
    p.rtext(float_str(orderinfo["amount"].toDouble(), 2));

    p.br();
    p.br();
    p.line();
    p.setFontSize(bs);
    p.setFontBold(false);
    if (noservice) {
        p.ltext(QString("* - %1").arg(tr_am("No service, no discount")), 0);
        p.br();
    }
    p.br();
    p.line();

//    if (fIdram[param_idram_id].length() > 0 && fBill){
//        p.br();
//        p.br();
//        p.br();
//        p.br();
//        p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
//        p.br();

//        int levelIndex = 1;
//        int versionIndex = 0;
//        bool bExtent = true;
//        int maskIndex = -1;
//        QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
//                .arg(fIdram[param_idram_name])
//                .arg(fIdram[param_idram_id]) //IDram ID
//                .arg(orderinfo["amount"].toDouble())
//                .arg(fHeader)
//                .arg(fIdram[param_idram_phone])
//                .arg(fIdram[param_idram_tips].toInt() == 1 ? "1" : "0")
//                .arg(fIdram[param_idram_tips].toInt() == 1 ? fIdram[param_idram_tips_wallet] : "");

//        CQR_Encode qrEncode;
//        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
//        if (!successfulEncoding) {
////            fLog.append("Cannot encode qr image");
//        }
//        int qrImageSize = qrEncode.m_nSymbleSize;
//        int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
//        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
//        encodeImage.fill(1);

//        for ( int i = 0; i < qrImageSize; i++ ) {
//            for ( int j = 0; j < qrImageSize; j++ ) {
//                if ( qrEncode.m_byModuleData[i][j] ) {
//                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
//                }
//            }
//        }

//        QPixmap pix = QPixmap::fromImage(encodeImage);
//        pix = pix.scaled(300, 300);
//        p.image(pix, Qt::AlignHCenter);
//        p.br();
//        /* End QRCode */
//    }


    if (orderinfo["amount"].toDouble() - orderinfo["amount_card"].toDouble() > 0.001) {
        p.ltext(tr_am("Payment, cash"), 0);
        p.rtext(float_str(orderinfo["amount"].toDouble(), 2));
        p.br();
    }
    if (orderinfo["amount_card"].toDouble() > 0.001) {
        p.ltext(tr_am("Payment, card"), 0);
        p.rtext(float_str(orderinfo["amount_card"].toDouble(), 2));
        p.br();
    }
    p.br();



    p.setFontSize(bs);
    p.setFontBold(true);
    p.ltext(tr_am("Thank you for visit!"), 0);
    p.br();
    p.ltext(QString("%1: %2").arg(tr_am("Sample")).arg(abs(orderinfo["print_qty"].toInt()) + 1), 0);
    p.br();
    p.br();
    p.br();
    p.br();

    p.setFontSize(46);
    p.ctext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();


    if (p.print(orderinfo["printer"].toString(), QPrinter::Custom) == false) {
        rm.putUByte(0);
        rm.putString(p.fErrorString);
        return false;
    }

    db[":print_qty"] = orderinfo["print_qty"].toInt() + 1;
    db.update("o_order", "id", orderid);

    return true;
}

bool printTax1(RawMessage &rm, Database &db, const QString &orderid)
{
    db[":order_id"] = orderid;
    if (db.exec("select h.settings, o.amount_inc, o.amount_card, o.amount, o.amount_inc_value from o_order o "
                "left join h_table t on t.id=o.table_id "
                "left join h_hall h on h.id=t.hall_id "
                "where o.id=:order_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(tr_am("Invalid order id"));
        return false;
    }
    double service_value = db.doubleValue("amount_inc_value");
    double total_service = db.doubleValue("amount_inc");
    double total_card = db.doubleValue("amount_card");
    double total = db.doubleValue("amount");
    db[":fhost"] = db.string("settings");
    if (db.exec("select * from sys_cnfapp where fkey like 'tax_%' and fhost=:fhost") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    QMap<QString, QString> taxconfig;
    while (db.next()) {
        taxconfig[db.string("fkey")] = db.string("fvalue");
    }
    if (taxconfig.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr_am("Fiscal machine not configured"));
        return false;
    }
    PrintTaxN pt(taxconfig["tax_ip_address"], taxconfig["tax_port"].toInt(),
            taxconfig["tax_password"], "false", "3", "3");

    db[":order_id"] = orderid;
    if (db.exec("select mt.adgcode, md.id, md.name, od.price, od.printed_qty, od.price_dec "
                "from o_dishes od "
                "left join me_dishes md on md.id=od.dish_id "
                "left join me_types mt on mt.id=md.type_id "
                "where od.state_id=1 and od.order_id=:order_id ") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    bool empty = true;
    while (db.next()) {
        empty = false;
        pt.addGoods(taxconfig["tax_dept"],
                db.string("adgcode"),
                db.string("id"),
                db.string("name"),
                db.doubleValue("price"),
                db.doubleValue("printed_qty"),
                db.doubleValue("price_dec") * 100);
    }
    if (empty) {
        rm.putUByte(0);
        rm.putString(tr_am("Empty order"));
        return false;
    }
    if (total_service > 0.001) {
        pt.addGoods(taxconfig["tax_dept"],
                    "5901",
                    "001",
                    QString("%1 %2%").arg(tr_am("Service"), float_str(service_value*100, 2)),
                    total_service, 1.0, 0);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(total_card, 0, jsonIn, jsonOut, err);
    db[":forder"] = orderid;
    db[":fdate"] = QDateTime::currentDateTime();
    db[":fin"] = jsonIn;
    db[":fout"] = jsonOut;
    db[":ferr"] = err;
    db[":fresult"] = result;
    db.insert("o_tax_log");
    if (result == pt_err_ok) {
        QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":fid"] = orderid;
        db.exec("delete from o_tax where fid=:fid");
        db[":fid"] = orderid;
        db[":ffiscal"] = fiscal;
        db[":fnumber"] = rseq;
        db[":fhvhh"] = hvhh;
        db[":fcash"] = total - total_card;
        db[":fcard"] = total_card;
        db[":json"] = jsonOut;
        db.insert("o_tax");
        return true;
    } else {
        rm.putUByte(0);
        rm.putString(jsonOut);
        return false;
    }
}

bool printServiceOnPrinter(const QMap<QString, QVariant> &order, const QMap<QString, QList<int> > &dishesToPrint, const QList<QMap<QString, QVariant> > &dishes)
{
    for (QMap<QString, QList<int> >::const_iterator pi = dishesToPrint.constBegin(); pi != dishesToPrint.constEnd(); pi++) {
        QStringList printerstore = pi.key().split(":", Qt::SkipEmptyParts);
        QString printer = printerstore.at(0);
        QString store = printerstore.at(1);
        QString side = printerstore.at(2);

        QFont font("Arial LatArm Unicode");
        font.setPointSize(20);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
        p.setFont(font);

        bool fBooking = false;
        if (order["state"].toInt() == ORDER_STATE_PREORDER_EMPTY
                || order["state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER
                || fBooking) {
            p.setFontSize(32);
            p.setFontBold(true);
            p.ctext("ՆԱԽԱԽԱՏՎԵՐ");
            p.br();
            p.ltext(order["datefor"].toDate().toString(FORMAT_DATE_TO_STR), 0);
            p.rtext(order["timefor"].toTime().toString(FORMAT_TIME_TO_STR));
            p.br();
            p.ltext(tr_am("Հյուրեր"), 0);
            p.rtext(order["guests"].toString());
            p.br();
            p.br();
            p.line();
            p.br();
        }

        if (order["reprint"].toInt() != 0) {
            p.setFontSize(34);
            p.setFontBold(true);
            p.ctext("ԿՐԿՆՈՒԹՅՈՒՆ");
            p.br();
            p.br();
        }

        p.setFontBold(false);
        p.setFontSize(26);
        p.br();
        p.ctext("ՆՈՐ ՊԱՏՎԵՐ");
        p.br();
        p.ltext("ՍԵՂԱՆ", 0);
        p.rtext(QString("%1/%2").arg(order["hallname"].toString(), order["tablename"].toString()));
        p.br();
        p.setFontSize(22);
        p.ltext(tr_am("ՊԱՏՎԵՐ"), 0);
        p.rtext(order["id"].toString());
        p.br();
        p.ltext("ԱՄՍԱԹԻՎ", 0);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext("ԺԱՄ", 0);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext("ՍՊԱՍԱՐԿՈՂ", 0);
        p.rtext(order["current_staff"].toString());
        p.br(p.fLineHeight + 2);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(2);

        p.setFontSize(30);
        for (const int &i: pi.value()) {
            const QMap<QString, QVariant> &o = dishes.at(i);

//            if (__c5config.getValue(param_print_dish_timeorder).toInt() == 1) {
//                p.ltext(QString("[%1] %2").arg(o["f_timeorder"].toString(), dbdish->name(o["f_dish"].toInt())), 0);
//            } else {
//
//            }
            p.ltext(QString("%1").arg(o["dishname"].toString()), 0);
            p.setFontBold(true);
            p.rtext(QString("%1").arg(float_str(o["qty"].toDouble(), 2)));
            p.setFontBold(false);

//            if (dbdish->isExtra(o["f_dish"].toInt())) {
//                p.br();
//                p.setFontSize(25);
//                p.ltext(QString("%1: %2")
//                        .arg(tr("Extra price"))
//                        .arg(float_str(o["f_price"].toDouble(), 2)), 0);
//            }

            if (o["comment"].toString().length() > 0) {
                p.br();
                p.setFontSize(25);
                p.setFontBold(true);
                p.ltext(o["comment"].toString(), 0);
                p.br();
                p.setFontSize(30);
                p.setFontBold(false);
            }

            p.br();
            p.line(0, p.fTop, p.fNormalWidth, p.fTop);
            p.br(1);
        }
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.setFontSize(20);
        p.ltext(QString("[%1]%2,%3").arg(side, store, printer), 0);
        p.setFontBold(true);
        p.br();

        if (!p.print(printer, QPrinter::Custom)) {
//            final = "FAIL";
        }

    }
    return true;
}

void readyDishes(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    QString orderid;
    rm.readUByte(version, in);
    rm.readString(orderid, in);
}

void createHeaderOfOrderV2(RawMessage &rm, Database &db, const QByteArray &in, int userid, int tableid)
{
    QString orderid = db.uuid();
    db.startTransaction();
    db[":f_id"] = tableid;
    db.exec("select f_hall from h_tables where f_id=:f_id");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Invalid table id");
        return;
    }
    int hall = db.integer("f_hall");
    db[":f_id"] = hall;
    db.exec("select f_id, f_prefix, f_counter + 1 as f_counter from h_halls where f_id=(select f_counterhall from h_halls where f_id=:f_id limit 1) for update");
    if (!db.next()) {
        db.rollback();
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    int hallid = db.integer("f_counter");
    QString prefix = db.integer("f_prefix");
    db[":f_counter"] = hallid;
    db.update("h_halls", "f_id", db.integer("f_id"));
    db.commit();

    db.startTransaction();
    db[":f_id"] = orderid;
    db.insert("o_header");
    db[":f_id"] = orderid;
    db.insert("o_header_options");
    db[":f_id"] = orderid;
    db.insert("o_tax");
    db[":f_id"] = orderid;
    db.insert("o_pay_cl");
    db[":f_id"] = orderid;
    db.insert("o_pay_room");
    db[":f_id"] = orderid;
    db.insert("o_header_flags");
    db[":f_id"] = orderid;
    db.insert("o_payment");
    db[":f_id"] = orderid;
    db.insert("o_preorder");
    db[":f_id"] = orderid;
    db[":f_checkin"] = QDate::currentDate();
    db[":f_checkout"] = QDate::currentDate();
    db.insert("o_header_hotel");
    db[":f_header"] = orderid;
    db[":f_date"] = QDate::currentDate();
    db[":f_1"] = 0;
    db[":f_2"] = 1;
    db.insert("o_Header_hotel_date");

    db[":f_id"] = hall;
    db.exec("SELECT h.f_id, h.f_name, s1.f_value AS f_menu, s2.f_value AS f_service "
                            "FROM h_halls h "
                            "LEFT JOIN s_settings_values s1 ON s1.f_settings=h.f_settings AND s1.f_key=9 "
                            "LEFT JOIN s_settings_values s2 ON s2.f_settings=h.f_settings AND s2.f_key=2 "
                            "where h.f_id=:f_id");
    if (db.next() == false) {
        db.rollback();
        rm.putUByte(0);
        rm.putString("No hall with id: " + QString::number(hallid));
        return;
    }
    double serviceFactor = db.doubleValue("f_service");

    db[":f_staff"] = userid;
    db[":f_table"] = tableid;
    db[":f_prefix"] = prefix;
    db[":f_hallid"] = hallid;
    db[":f_dateopen"] = QDate::currentDate();
    db[":f_timeopen"] = QTime::currentTime();
    db[":f_currentstaff"] = userid;
    db[":f_state"] = 1; //ORDER_STATE_OPEN
    db[":f_precheck"] = 0;
    db[":f_print"] = 0;
    db[":f_guests" ]= 1;
    db[":f_comment"] = "";
    db[":f_hall"] = hall;
    db[":f_amounttotal"] = 0;
    db[":f_amountcash"] = 0;
    db[":f_amountcard"] = 0;
    db[":f_amountbank"] = 0;
    db[":f_amountother"] = 0;
    db[":f_amountservice"] = 0;
    db[":f_amountdiscount"] = 0;
    db[":f_servicefactor"] = serviceFactor;
    db[":f_discountfactor"] = 0;
    db.update("o_header", "f_id", orderid);
    db.commit();

    quint8 neworder = 1;
    rm.putUByte(1);
    rm.putUByte(neworder);
    rm.putString(orderid);
}

bool countOrder2(RawMessage &rm, Database &db, const QString &orderid)
{
    db[":f_header"] = orderid;
    db[":f_state"] = 1;
    if (db.exec("select * from o_body where f_header=:f_header and f_state=:f_state") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    double total = 0, total_service = 0, total_disc = 0;
    while (db.next()) {
        double t = db.doubleValue("f_qty1") * db.doubleValue("f_price");
        double ts = t * db.doubleValue("f_service");
        t += ts;
        double td = t * db.doubleValue("f_discount");
        t -= td;
        total += t;
        total_service += ts;
        total_disc += td;
    }
    db[":f_amounttotal"] = total;
    db[":f_amountservice"] = total_service;
    db[":f_amountdiscount"] = total_disc;
    if (db.update("o_header", "f_id", orderid) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }
    return true;
}

bool printBill2(RawMessage &rm, Database &db, const QString &orderid)
{
    QString err;
    PrintBill pb;
    if (!pb.printBill2(&db, false, orderid, "local", 0, err)) {
        rm.putUByte(0);
        rm.putString(err);
        return false;
    }
    return true;
}

void processScanDiscount(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    QString orderid, cardcode;

    rm.readUByte(version, in);
    rm.readString(orderid, in);
    rm.readString(cardcode, in);

    int cardmode = 0;
    double cardvalue = 0;
    int cardid = 0;
    switch (version) {
    case 2:
        db[":f_code"] = cardcode;
        if (db.exec("select * from b_cards_discount where f_code=:f_code") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (!db.next()) {
            rm.putUByte(0);
            rm.putString(ntr("Unknown card code", ntr_am));
            return;
        }
        if (db.integer("f_active") == 0) {
            rm.putUByte(0);
            rm.putString(ntr("Card is not active", ntr_am));
            return;
        }
        if (db.date("f_dateend") < QDate::currentDate()) {
            rm.putUByte(0);
            rm.putString(ntr("Card expired", ntr_am));
            return;
        }
        cardid = db.integer("f_id");
        cardmode = db.integer("f_mode");
        cardvalue = db.doubleValue("f_value");

        db[":f_id"] = orderid;
        if (db.exec("select * from o_header where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.doubleValue("f_discountfactor") > 0.001) {
            rm.putUByte(0);
            rm.putString(ntr("Discount already applied", ntr_am));
            return;
        }
        db.startTransaction();
        db[":f_discountfactor"] = cardvalue;
        if (db.update("o_header", "f_id", orderid) == false){
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        db[":f_id"] = cardid;
        db[":f_state"] = 0;
        if (db.exec("update b_cards_discount set f_active=0 where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        db[":f_id"] = orderid;
        db[":f_card"] = cardid;
        db[":f_type"] = cardmode;
        db[":f_value"] = cardvalue;
        if (db.insert("b_history") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        db.commit();
        rm.putUByte(1);
        rm.putString(ntr("Thank you, discount applied", ntr_am));
        return;
    }
}
