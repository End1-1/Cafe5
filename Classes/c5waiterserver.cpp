#include "c5waiterserver.h"
#include "c5serverhandler.h"
#include "c5config.h"
#include "c5socketmessage.h"
#include "c5printservicethread.h"
#include "c5utils.h"
#include <QDebug>
#include <QJsonArray>
#include <QMutex>

QMutex mutex;

C5WaiterServer::C5WaiterServer(const QJsonObject &o) :
    fIn(o)
{

}

void C5WaiterServer::reply(QJsonObject &o)
{
    C5ServerHandler srh;
    QMap<QString, QVariant> bv;

    int cmd = fIn["cmd"].toInt();
    qDebug() << "handle socket from dlgface" << cmd;
    switch (cmd) {
    case sm_hall: {
        QJsonArray jHall;
        srh.getJsonFromQuery("select f_id, f_name from h_halls", jHall);
        QJsonArray jTables;
        srh.getJsonFromQuery("select t.f_id, t.f_hall, t.f_name, t.f_lock, t.f_lockSrc, \
                            h.f_id as f_header, concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_staffName, \
                            h.f_amountTotal as f_amount, \
                            date_format(h.f_dateOpen, '%d.%m.%Y') as f_dateOpen, h.f_timeOpen \
                            from h_tables t \
                            left join o_header h on h.f_table=t.f_id and h.f_state=1 \
                            left join s_user u on u.f_id=h.f_staff \
                            order by f_id", jTables);
        srh.getJsonFromQuery("select f_id, f_hall, f_name, f_lock, f_lockSrc from h_tables order by f_id", jTables);
        o["reply"] = 1;
        o["halls"] = jHall;
        o["tables"] = jTables;
        break;
    }
    case sm_menu: {
        QJsonArray jMenu;
        QString query = "select d.f_id as f_dish, mn.f_name as menu_name, p1.f_name as part1, p2.f_name as part2, d.f_name, \
                m.f_price, m.f_store, m.f_print1, m.f_print2, d.f_remind, \
                s.f_name as f_storename \
                from d_menu m \
                left join d_menu_names mn on mn.f_id=m.f_menu \
                left join d_dish d on d.f_id=m.f_dish \
                left join d_part2 p2 on p2.f_id=d.f_part \
                left join d_part1 p1 on p1.f_id=p2.f_part \
                left join c_storages s on s.f_id=m.f_store ";
        srh.getJsonFromQuery(query, jMenu);
        o["reply"] = 1;
        o["menu"] = jMenu;
        break;
    }
    case sm_checkuser: {
        QJsonArray jUser;
        bv[":f_altPassword"] = fIn["pass"].toString();
        bv[":f_state"] = 1;
        srh.getJsonFromQuery("select f_id, f_group, f_first, f_last from s_user where f_altPassword=:f_altPassword and f_state=:f_state", jUser, bv);
        o["reply"] = jUser.count();
        o["user"] = jUser;
        break;
    }
    case sm_waiterconf: {
        QJsonArray jConf;
        jConf.insert(0, QJsonValue("{\"default_menu\":\"M1\"}"));
        o["reply"] = 1;
        o["conf"] = jConf;
        break;
    }
    case sm_opentable: {
        QMutexLocker ml(&mutex);
        QJsonArray jt;
        bv[":f_id"] = fIn["table"].toInt();
        srh.getJsonFromQuery("select * from h_tables where f_id=:f_id", jt, bv);
        if (jt.count() > 0) {
            if (jt.at(0)["f_lock"].toString().toInt() == 0) {
                o["reply"] = 1;
                srh.fDb[":f_lock"] = 1;
                srh.fDb[":f_lockSrc"] = fIn["host"].toString();
                srh.fDb[":f_id"] = fIn["table"].toInt();
                srh.fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
                QJsonArray jo;
                QJsonArray jb;
                bv[":f_table"] = fIn["table"].toInt();
                bv[":f_state"] = ORDER_STATE_OPEN;
                srh.getJsonFromQuery("select t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, \
                    o.* \
                    from o_header o \
                    left join h_tables t on t.f_id=o.f_table \
                    left join s_user s on s.f_id=o.f_staff \
                    where o.f_table=:f_table and o.f_state=:f_state \
                    order by o.f_id \
                    limit 1 ", jo, bv);
                if (jo.count() == 0) {
                    QJsonObject jh;
                    srh.fDb[":f_id"] = fIn["table"].toInt();
                    srh.fDb.exec("select f_name from h_table where f_id=:f_id");
                    if (srh.fDb.nextRow()) {
                        jh["f_tableName"] = srh.fDb.getString(0);
                    }
                    jh["f_id"] = "0";
                    jh["f_hall"] = jt.at(0)["f_hall"];
                    jh["f_table"] = QString::number(fIn["table"].toInt());
                    jh["f_state"] = QString::number(ORDER_STATE_OPEN);
                    jh["f_dateopen"] = current_date;
                    jh["f_timeopen"] = current_time;
                    jo.append(jh);
                } else {
                    bv[":f_header"] = jo.at(0).toObject()["f_id"].toString().toInt();
                    srh.getJsonFromQuery("select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, d.f_name as f_name, \
                                         ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, \
                                         ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, \
                                         s.f_name as f_storename \
                                         from o_body ob \
                                         left join d_dish d on d.f_id=ob.f_dish \
                                         left join d_part2 dp2 on dp2.f_id=d.f_part \
                                         left join d_part1 dp1 on dp1.f_id=dp2.f_part \
                                         left join c_storages s on s.f_id=ob.f_store \
                                         where ob.f_header=:f_header", jb, bv);
                }
                o["header"] = jo;
                o["body"] = jb;
                o["table"] = jt;
            } else {
                o["reply"] = 2;
                o["msg"] = tr("Table locked by ") + jt.at(0)["f_locksrc"].toString();
            }
        } else {
            o["reply"] = 0;
            o["msg"] = tr("Program error, unknown table id or connection error");
        }
        ml.unlock();
        break;
    }
    case sm_saveorder: {
        QJsonObject jh = fIn["header"].toObject();
        QJsonArray ja = fIn["body"].toArray();
        if (jh.contains("unlocktable")) {
            if (jh["unlocktable"].toString().toInt() > 0) {
                srh.fDb[":f_lock"] = 0;
                srh.fDb[":f_lockSrc"] = "";
                srh.fDb[":f_id"] = jh["f_table"].toString();
                srh.fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
            }
        }
        if (jh["f_id"].toString().toInt() == 0 && ja.count() > 0) {
            srh.fDb[":f_id"] = 0;
            srh.fDb[":f_dateOpen"] = QDate::fromString(jh["f_dateopen"].toString(), FORMAT_DATE_TO_STR);
            srh.fDb[":f_timeOpen"] = QTime::fromString(jh["f_timeopen"].toString(), FORMAT_TIME_TO_STR);
            srh.fDb[":f_prefix"] = jh["f_prefix"].toString();
            jh["f_id"] = QString::number(srh.fDb.insert("o_header"));
        }
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jb = ja.at(i).toObject();
            saveDish(jh, jb, srh.fDb);
            ja[i] = jb;
        }
        if (ja.count() > 0) {
            srh.fDb[":f_comment"] = jh["f_comment"].toString();
            srh.fDb[":f_staff"] = jh["f_staff"].toString().toInt();
            srh.fDb[":f_hall"] = jh["f_hall"].toString().toInt();
            srh.fDb[":f_table"] = jh["f_table"].toString().toInt();
            srh.fDb[":f_state"] = jh["f_state"].toString().toInt();
            srh.fDb[":f_amountTotal"] = jh["f_amounttotal"].toString().toDouble();
            srh.fDb[":f_amountCash"] = jh["f_amountcache"].toString().toDouble();
            srh.fDb[":f_amountCard"] = jh["f_amountcard"].toString().toDouble();
            srh.fDb[":f_amountBank"] = jh["f_amountbank"].toString().toDouble();
            srh.fDb[":f_amountOther"] = jh["f_amountother"].toString().toDouble();
            srh.fDb[":f_amountService"] =jh["f_amountservice"].toString().toDouble();
            srh.fDb[":f_amountDiscount"] = jh["f_amountdiscount"].toString().toDouble();
            srh.fDb.update("o_header", where_id(jh["f_id"].toString()));
        }
        o = fIn;
        o["header"] = jh;
        o["body"] = ja;
        o["reply"] = 1;
        break;
    }
    case sm_printservice: {
        o["reply"] = 1;
        QJsonArray ji = fIn["body"].toArray();
        QJsonArray jp;
        for (int i = 0; i < ji.count(); i++ ) {
            QJsonObject jo = ji[i].toObject();            
            if (jo["f_qty2"].toString().toDouble() < jo["f_qty1"].toString().toDouble()) {
                jo["f_qtyprint"] = jo["f_qty1"].toString().toDouble() - jo["f_qty2"].toString().toDouble();
                jp.append(jo);
            }
            jo["f_qty2"] = jo["f_qty1"];
            saveDish(fIn["header"].toObject(), jo, srh.fDb);
            ji[i] = jo;
        }
        C5PrintServiceThread *ps = new C5PrintServiceThread(fIn["header"].toObject(), jp);
        ps->start();
        o["body"] = ji;
        break;
    }
    case sm_printreceipt: {
        QString err;
        QJsonArray body = fIn["body"].toArray();
        for (int i = 0; i < body.count(); i++) {
            QJsonObject jo = body[i].toObject();
            if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
                err += tr("All service must be printed");
                break;
            }
        }
        o["reply"] = err.isEmpty() ? 1 : 0;
        o["msg"] = err;
        break;
    }
    default:
        o["reply"] = 0;
        o["msg"] = QString("%1: %2").arg(tr("Unknown command for socket handler from dlgface")).arg(cmd);
        break;
    }
}

void C5WaiterServer::saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db)
{
    if (o["f_id"].toString().toInt() == 0) {
        db[":f_id"] = 0;
        o["f_id"] = QString::number(db.insert("o_body"));
    }
    db[":f_header"] = h["f_id"].toString().toInt();
    db[":f_state"] = o["f_state"].toString().toInt();
    db[":f_dish"] = o["f_dish"].toString().toInt();
    db[":f_qty1"] = o["f_qty1"].toString().toDouble();
    db[":f_qty2"] = o["f_qty2"].toString().toDouble();
    db[":f_price"] = o["f_price"].toString().toDouble();
    db[":f_service"] = o["f_service"].toString().toDouble();
    db[":f_discount"] = o["f_discount"].toString().toDouble();
    db[":f_total"] = o["f_total"].toString().toDouble();
    db[":f_store"] = o["f_store"].toString().toInt();
    db[":f_print1"] = o["f_print1"].toString();
    db[":f_print2"] = o["f_print2"].toString();
    db[":f_comment"] = o["f_comment"].toString();
    db[":f_remind"] = o["f_remind"].toString().toInt();
    db.update("o_body", where_id(o["f_id"].toString()));
}
