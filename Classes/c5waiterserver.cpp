#include "c5waiterserver.h"
#include "c5serverhandler.h"
#include "c5config.h"
#include "c5socketmessage.h"
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
        srh.getJsonFromQuery("select f_id, f_hall, f_name, f_lock, f_lockSrc from h_tables order by f_id", jTables);
        o["reply"] = 1;
        o["halls"] = jHall;
        o["tables"] = jTables;
        break;
    }
    case sm_menu: {
        QJsonArray jMenu;
        QString query = "select mn.f_name as menu_name, p1.f_name as part1, p2.f_name as part2, d.f_name, \
                m.f_price, m.f_store, m.f_print1, m.f_print2, d.f_remind \
                from d_menu m \
                left join d_menu_names mn on mn.f_id=m.f_menu \
                left join d_dish d on d.f_id=m.f_dish \
                left join d_part2 p2 on p2.f_id=d.f_part \
                left join d_part1 p1 on p1.f_id=p2.f_part ";
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
                bv[":f_table"] = fIn["table"].toInt();
                bv[":f_state"] = ORDER_STATE_OPEN;
                srh.getJsonFromQuery("select * from o_header where f_table=:f_table and f_state=:f_state", jo, bv);
                if (jo.count() == 0) {
                    QJsonObject jh;
                    jh["f_id"] = 0;
                    jh["f_hall"] = jt.at(0)["f_hall"];
                    jh["f_table"] = QString::number(fIn["table"].toInt());
                    jh["f_state"] = QString::number(ORDER_STATE_OPEN);
                    jh["f_dateOpen"] = current_date;
                    jh["f_timeOpen"] = current_time;
                    jo.append(jh);
                }
                o["header"] = jo;
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
        o["reply"] = 1;
        QJsonObject jh = fIn["header"].toObject();
        if (jh.contains("unlocktable")) {
            if (jh["unlocktable"].toString().toInt() > 0) {
                srh.fDb[":f_lock"] = 0;
                srh.fDb[":f_lockSrc"] = "";
                srh.fDb[":f_id"] = jh["f_table"].toString().toInt();
                srh.fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
            }
        }
        if (jh["id"].toInt() == 0) {
            srh.fDb[":f_id"] = 0;
            jh["id"] = srh.fDb.insert("o_header");
        }
        o["ooo"] = fIn;
        break;
    }
    default:
        o["reply"] = 0;
        o["msg"] = QString("%1: %2").arg(tr("Unknown command for socket handler from dlgface")).arg(cmd);
        break;
    }
}
