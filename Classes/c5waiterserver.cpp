#include "c5waiterserver.h"
#include "c5config.h"
#include "c5socketmessage.h"
#include "c5printservicethread.h"
#include "c5printreceiptthread.h"
#include "c5printremovedservicethread.h"
#include "c5permissions.h"
#include "printtaxn.h"
#include "c5waiterorderdoc.h"
#include "c5utils.h"
#include <QDebug>
#include <QJsonArray>
#include <QMutex>
#include <QDir>
#include <QTcpSocket>
#include <QPrinter>
#include <QLibrary>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>

static QMutex mutex;
typedef void (*caption)(QString &str);
typedef void (*json)(C5Database &db, const QJsonObject &params, QJsonArray &jarr);
typedef void (*translator)(QTranslator &trans);

C5WaiterServer::C5WaiterServer(const QJsonObject &o, QTcpSocket *socket) :
    fIn(o)
{
    fSocket = socket;
    fPeerAddress = QHostAddress(fSocket->peerAddress().toIPv4Address()).toString();
}

void C5WaiterServer::reply(QJsonObject &o)
{
    C5ServerHandler srh;
    QMap<QString, QVariant> bv;

    int cmd = fIn["cmd"].toInt();
    switch (cmd) {
    case sm_hall: {
        QJsonArray jHall;
        QString hallFilter = fIn["hall"].toString();
        srh.getJsonFromQuery(QString("select f_id, f_name, f_settings from h_halls %1")
                             .arg(hallFilter.isEmpty() ? "" : " where f_id in (" + hallFilter + ")"), jHall);
        QJsonArray jTables;
        srh.getJsonFromQuery(QString("select t.f_id, t.f_hall, t.f_name, t.f_lock, t.f_lockSrc, \
                            h.f_id as f_header, concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_staffName, \
                            h.f_amountTotal as f_amount, h.f_print, \
                            date_format(h.f_dateOpen, '%d.%m.%Y') as f_dateOpen, h.f_timeOpen \
                            from h_tables t \
                            left join o_header h on h.f_table=t.f_id and h.f_state=1 \
                            left join s_user u on u.f_id=h.f_staff  %1 \
                            order by f_id")
                .arg(hallFilter.isEmpty() ? "" : " where t.f_hall in (" + hallFilter + ") "), jTables);
        //srh.getJsonFromQuery("select f_id, f_hall, f_name, f_lock, f_lockSrc from h_tables order by f_id", jTables);
        o["reply"] = 1;
        o["halls"] = jHall;
        o["tables"] = jTables;
        break;
    }
    case sm_menu: {
        QJsonArray jMenu;
        QString query = "select d.f_id as f_dish, mn.f_name as menu_name, p1.f_name as part1, p2.f_name as part2, p2.f_adgCode, d.f_name, \
                m.f_price, m.f_store, m.f_print1, m.f_print2, d.f_remind, \
                s.f_name as f_storename, d.f_color as dish_color, p2.f_color as type_color \
                from d_menu m \
                left join d_menu_names mn on mn.f_id=m.f_menu \
                left join d_dish d on d.f_id=m.f_dish \
                left join d_part2 p2 on p2.f_id=d.f_part \
                left join d_part1 p1 on p1.f_id=p2.f_part \
                left join c_storages s on s.f_id=m.f_store \
                where m.f_state=1 ";
        srh.getJsonFromQuery(query, jMenu);
        QJsonArray jMenuNames;
        srh.getJsonFromQuery("select f_id, f_name from d_menu_names", jMenuNames);
        o["reply"] = 1;
        o["menu"] = jMenu;
        o["menunames"] = jMenuNames;
        break;
    }
    case sm_checkuser: {
        QJsonArray jUser;
        bv[":f_altPassword"] = fIn["pass"].toString();
        bv[":f_state"] = 1;
        srh.getJsonFromQuery("select f_id, f_group, f_first, f_last from s_user where f_altPassword=:f_altPassword and f_state=:f_state", jUser, bv);
        if (jUser.count() > 0) {
            if (checkPermission(jUser[0].toObject()["f_id"].toString().toInt(), cp_t5_waiter, srh.fDb)) {
                o["reply"] = jUser.count();
                o["user"] = jUser;
                srh.fDb[":f_group"] = jUser[0].toObject()["f_group"].toString().toInt();
                srh.fDb.exec("select f_key from s_user_access where f_group=:f_group and f_value=1");
                QString permissions;
                bool first = true;
                while (srh.fDb.nextRow()) {
                    if (first) {
                        first = false;
                    } else {
                        permissions += ",";
                    }
                    permissions += srh.fDb.getString(0);
                }
                o["f_permissions"] = permissions;
            } else {
                o["reply"] = 0;
            }
        } else {
            o["reply"] = 0;
        }
        break;
    }
    case sm_dishremovereason: {
        QJsonArray jr;
        srh.getJsonFromQuery("select f_name from o_dish_remove_reason", jr);
        o["reply"] = 1;
        o["reasons"] = jr;
        break;
    }
    case sm_waiterconf: {
        QJsonObject jConf;
        srh.fDb[":f_ip"] = fPeerAddress;;
        srh.fDb.exec("select * from s_station_conf where f_ip=:f_ip");
        if (srh.fDb.nextRow()) {
           o["date_cash"] = srh.fDb.getString("f_datecash");
           o["date_auto"] = srh.fDb.getString("f_datecashauto");
           srh.fDb[":f_settings"] = srh.fDb.getInt("f_conf");
           srh.fDb.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
           while (srh.fDb.nextRow()) {
               jConf[srh.fDb.getString(0)] = srh.fDb.getString(1);
           }
           srh.fDb[":f_id"] = jConf[QString::number(param_default_menu)].toString();
           srh.fDb.exec("select f_name from d_menu_names where f_id=:f_id");
           if (srh.fDb.nextRow()) {
               jConf[QString::number(param_default_menu_name)] = srh.fDb.getString(0);
           } else {
               jConf[QString::number(param_default_menu_name)] = "NO MENU DEFINED";
           }
        } else {
            srh.fDb[":f_station"] = fIn["station"];
            srh.fDb[":f_conf"] = 1;
            srh.fDb[":f_ip"] = QHostAddress(fSocket->peerAddress().toIPv4Address()).toString();
            srh.fDb.insert("s_station_conf", false);
        }
        QJsonArray jOther;
        srh.getJsonFromQuery("select f_settings, f_key, f_value from s_settings_values", jOther);
        o["reply"] = 1;
        o["conf"] = jConf;
        o["otherconf"] = jOther;
        break;
    }
    case sm_opentable: {
        QMutexLocker ml(&mutex);
        QJsonArray jt;
        QJsonObject jh;
        QJsonArray jb;
        bv[":f_id"] = fIn["table"].toInt();
        srh.getJsonFromQuery("select * from h_tables where f_id=:f_id", jt, bv);
        if (jt.count() > 0) {
            if (jt.at(0).toObject()["f_lock"].toString().toInt() == 0) {
                o["reply"] = 1;
                openActiveOrder(jh, jb, jt, srh);
                o["header"] = jh;
                o["body"] = jb;
                o["table"] = jt;
            } else {
                if (jt.at(0).toObject()["f_locksrc"].toString() == fIn["host"].toString()) {
                    o["table"] = fIn["table"];
                    o["reply"] = 3;
                    o["msg"] = tr("Table wasnt unlocked correctly, try again");
                    srh.fDb[":f_lock"] = 0;
                    srh.fDb[":f_locksrc"] = "";
                    srh.fDb.update("h_tables", where_id(fIn["table"].toInt()));
                } else {
                    o["reply"] = 2;
                    o["msg"] = tr("Table locked by ") + jt.at(0).toObject()["f_locksrc"].toString();
                }
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
        saveOrder(jh, ja, srh.fDb);
        o = fIn;
        o["header"] = jh;
        o["body"] = ja;
        o["reply"] = 1;
        break;
    }
    case sm_printservice: {
        o["reply"] = 1;
        QJsonArray ji = fIn["body"].toArray();
        QJsonObject jh = fIn["header"].toObject();
        QJsonArray jp;
        saveOrder(jh, ji, srh.fDb);
        for (int i = 0; i < ji.count(); i++ ) {
            QJsonObject jo = ji[i].toObject();
            if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
                continue;
            }
            if (jo["f_qty2"].toString().toDouble() < jo["f_qty1"].toString().toDouble()) {
                jo["f_qtyprint"] = float_str(jo["f_qty1"].toString().toDouble() - jo["f_qty2"].toString().toDouble(), 2);
                jp.append(jo);
            }
            jo["f_qty2"] = jo["f_qty1"];
            saveDish(jh, jo, srh.fDb);
            ji[i] = jo;
        }
        if (jp.count() > 0) {
            if (jh["f_print"].toString().toInt() > 0) {
                jh["f_print"] = QString::number(jh["f_print"].toString().toInt() * -1);
                srh.fDb[":f_print"] = jh["f_print"].toString().toInt();
                srh.fDb.update("o_header", where_id(jh["f_id"].toString().toInt()));
            }
        }
        C5PrintServiceThread *ps = new C5PrintServiceThread(jh, jp);
        ps->start();
        o["body"] = ji;
        o["header"] = jh;
        break;
    }
    case sm_printreceipt: {
        QString err;
        QJsonArray jb = fIn["body"].toArray();
        QJsonObject jh = fIn["header"].toObject();
        printReceipt(jh, jb, err, srh);
        o["reply"] = err.isEmpty() ? 1 : 0;
        o["msg"] = err;
        o["header"] = jh;
        o["body"] = jb;
        break;
    }
    case sm_bill: {
        QString err;
        QJsonArray jb = fIn["body"].toArray();
        QJsonObject jh = fIn["header"].toObject();
        printBill(jh, jb, err, srh);
        o["reply"] = err.isEmpty() ? 1 : 0;
        o["msg"] = err;
        o["header"] = jh;
        o["body"] = jb;
        break;
    }
    case sm_closeorder: {
        QString err;
        QJsonArray jb = fIn["body"].toArray();
        QJsonObject jh = fIn["header"].toObject();
        // CHECKING ALL ITEMS THAT WAS PRINTED
        for (int i = 0; i < jb.count(); i++) {
            QJsonObject jo = jb[i].toObject();
            if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
                continue;
            }
            if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
                err += tr("All service must be complited");
                break;
            }
        }
        if (jh["f_print"].toString().toInt() < 1) {
            err += tr("Receipt was not printed");
        }
        QDate dateCash = QDate::currentDate();
        bool isAuto;
        srh.fDb[":f_ip"] = fPeerAddress;
        srh.fDb.exec("select f_dateCash, f_dateCashAuto from s_station_conf where f_ip=:f_ip");
        if (srh.fDb.nextRow()) {
            isAuto = srh.fDb.getInt(1) == 1;
            if (!isAuto) {
                dateCash = srh.fDb.getDate(0);
            }
        }
        if (err.isEmpty()) {
            jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
            jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR);
            jh["f_datecash"] = dateCash.toString(FORMAT_DATE_TO_STR);
            srh.fDb[":f_state"] = ORDER_STATE_CLOSE;
            srh.fDb[":f_dateClose"] = QDate::currentDate();
            srh.fDb[":f_dateCash"] = QDate::fromString(jh["f_datecash"].toString(), FORMAT_DATE_TO_STR);
            srh.fDb[":f_timeClose"] = QTime::fromString(jh["f_timeclose"].toString(), FORMAT_TIME_TO_STR);
            srh.fDb[":f_staff"] = jh["f_currentstaff"].toString().toInt();
            srh.fDb.update("o_header", where_id(jh["f_id"].toString()));
            srh.fDb[":f_lock"] = 0;
            srh.fDb[":f_lockSrc"] = "";
            srh.fDb.update("h_tables", where_id(jh["f_table"].toString().toInt()));

            C5Database fDD(C5Config::dbParams().at(0), C5Config::hotelDatabase(), C5Config::dbParams().at(2), C5Config::dbParams().at(3));
            C5WaiterOrderDoc w(jh, jb, srh.fDb);
            //w.transferToHotel(fDD, err);
        }
        o["reply"] = err.isEmpty() ? 1 : 0;
        o["msg"] = err;
        break;
    }
    case sm_creditcards: {
        QJsonArray ja;
        srh.getJsonFromQuery("select * from o_credit_card", ja);
        o["reply"] = 1;
        o["cards"] = ja;
        break;
    }
    case sm_discount: {
        QJsonArray ja;
        bv[":f_order"] = fIn["order"].toString();
        srh.getJsonFromQuery("select * from b_history where f_order=:f_order", ja, bv);
        if (ja.count() > 0) {
            o["reply"] = 0;
            o["msg"] = tr("Bonus system alreay exists for this order");
            break;
        }
        QJsonObject jo;
        //Check discount type
        bv[":f_code"] = fIn["code"].toString();
        srh.getJsonFromQuery("select f_id, f_value from b_cards_discount where f_code=:f_code", ja, bv);
        if (ja.count() > 0) {
            o["reply"] = 1;
            o["type"] = CARD_TYPE_DISCOUNT;
            srh.fDb[":f_type"] = CARD_TYPE_DISCOUNT;
            srh.fDb[":f_value"] = ja.at(0).toObject()["f_value"].toString().toDouble();
            srh.fDb[":f_card"] = ja.at(0).toObject()["f_id"].toString().toInt();
            srh.fDb[":f_data"] = 0;
            srh.fDb[":f_order"] = fIn["order"];
            jo["f_id"] = QString::number(srh.fDb.insert("b_history"));
            jo["f_type"] = QString::number(CARD_TYPE_DISCOUNT);
            jo["f_value"] = ja.at(0).toObject()["f_value"].toString();
            o["card"] = jo;
            break;
        } else {

        }
        o["reply"] = 0;
        o["msg"] = tr("Cannot find card");
        break;
    }
    case sm_dishcomment: {
        QJsonArray ja;
        srh.getJsonFromQuery("select f_name from d_dish_comment", ja);
        o["reply"] = 1;
        o["comments"] = ja;
        break;
    }
    case sm_dailycommon: {
        QJsonArray ja;
        bv[":f_datecash1"] = QDate::fromString(fIn["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
        bv[":f_datecash2"] = QDate::fromString(fIn["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
        bv[":f_state"] = ORDER_STATE_CLOSE;
        srh.getJsonFromQuery("select oh.f_id, concat(oh.f_prefix, oh.f_hallid) as f_order, date_format(oh.f_datecash, '%d.%m.%Y') as f_datecash, oh.f_timeclose, "
                             "h.f_name as f_hall, t.f_name as f_table, concat(u.f_last, ' ', u.f_first) as f_staff,"
                             "oh.f_amounttotal "
                             "from o_header oh "
                             "left join h_halls h on h.f_id=oh.f_hall "
                             "left join h_tables t on t.f_id=oh.f_table "
                             "left join s_user u on u.f_id=oh.f_staff "
                             "where oh.f_state=:f_state and oh.f_datecash between :f_datecash1 and :f_datecash2 "
                             "order by oh.f_timeclose", ja, bv);
        o["reply"] = 0;
        o["report"] = ja;
        break;
    }
    case sm_reports: {
        QJsonArray ja;
        QDir dir(qApp->applicationDirPath() + "/waiterreports");
        QStringList dll = dir.entryList(QStringList() << "*.dll", QDir::Files);
        foreach (QString s, dll) {
            QLibrary l("./waiterreports/" + s);
            if (!l.load()) {
                continue;
            }
            caption c = (caption) l.resolve("caption");
            if (!c) {
                l.unload();
                continue;
            }
            translator t = (translator) (l.resolve("translator"));
            if (!t) {
                l.unload();
                o["reply"] = 1;
                o["msg"] = QString("translator %1 %2").arg(tr("entry point is missing in")).arg(s);
                break;
            }
            QTranslator trans;
            t(trans);
            qApp->installTranslator(&trans);
            QJsonObject jo;
            QString caption;
            c(caption);
            jo["caption"] = caption;
            jo["file"] = s;
            ja.append(jo);
            l.unload();
        }
        o["reply"] = 0;
        o["reports"] = ja;
        break;
    }
    case sm_waiter_report: {
        QString filename = fIn["file"].toString();
        QLibrary l(qApp->applicationDirPath() + "/waiterreports/" + filename);
        if (!l.load()) {
            o["reply"] = 1;
            o["msg"] = QString("%1 %2").arg(tr("Could not load")).arg(filename);
            break;
        }
        translator t = (translator) (l.resolve("translator"));
        if (!t) {
            l.unload();
            o["reply"] = 1;
            o["msg"] = QString("translator %1 %2").arg(tr("entry point is missing in")).arg(filename);
            break;
        }
        QTranslator trans;
        t(trans);
        qApp->installTranslator(&trans);
        json j = (json) l.resolve("json");
        if (!j) {
            l.unload();
            o["reply"] = 1;
            o["msg"] = QString("json %1 %2").arg(tr("entry point is missing in")).arg(filename);
            break;
        }
        QJsonObject jo;
        jo["date1"] = fIn["date1"];
        jo["date2"] = fIn["date2"];
        o["reply"] = 0;
        QJsonArray report;
        j(srh.fDb, jo, report);
        QJsonObject jp;
        jp["pagesize"] = (int) QPrinter::Custom;
        jp["printer"] = "local";
        jp["cmd"] = "print";
        report.append(jp);
        l.unload();
        o["reply"] = 0;
        o["report"] = report;
        break;
    }
    case sm_log:
        remember(fIn);
        break;
    case sm_version: {
        QJsonArray versions;
        getVersions(srh.fDb, versions);
        o["reply"] = 0;
        o["versions"] = versions;
        break;
    }
    default:
        o["reply"] = 0;
        o["msg"] = QString("%1: %2").arg(tr("Unknown command for socket handler from dlgface")).arg(cmd);
        break;
    }
}

bool C5WaiterServer::checkPermission(int user, int permission, C5Database &db)
{
    db[":f_id"] = user;
    db.exec("select f_group from s_user where f_id=:f_id");
    if (!db.nextRow()) {
        return false;
    }
    if (db.getInt(0) == 1) {
        return true;
    }
    db[":f_group"] = db.getInt(0);
    db[":f_key"] = permission;
    db.exec("select * from s_user_access where f_group=:f_group and f_key=:f_key and f_value=1");
    return db.nextRow();
}

void C5WaiterServer::openActiveOrder(QJsonObject &jh, QJsonArray &jb, QJsonArray &jt, C5ServerHandler &srh)
{
    QMap<QString, QVariant> bv;
    srh.fDb[":f_lock"] = 1;
    srh.fDb[":f_lockSrc"] = fIn["host"].toString();
    srh.fDb[":f_id"] = fIn["table"].toInt();
    srh.fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
    QJsonArray jo;
    bv[":f_table"] = fIn["table"].toInt();
    bv[":f_state"] = ORDER_STATE_OPEN;
    srh.getJsonFromQuery("select o.f_id from o_header o  where o.f_table=:f_table and o.f_state=:f_state limit 1 ", jo, bv);
    if (jo.count() == 0) {
        srh.fDb[":f_id"] = fIn["table"].toInt();
        srh.fDb.exec("select f_name from h_tables where f_id=:f_id");
        if (srh.fDb.nextRow()) {
            jh["f_tablename"] = srh.fDb.getString(0);
        }
        srh.fDb[":f_id"] = jt.at(0).toObject()["f_hall"].toString();
        srh.fDb.exec("select f_name from h_halls where f_id=:f_id");
        if (srh.fDb.nextRow()) {
            jh["f_hallname"] = srh.fDb.getString(0);
        }
        jh["f_id"] = "";
        jh["f_hall"] = jt.at(0).toObject()["f_hall"];
        jh["f_table"] = QString::number(fIn["table"].toInt());
        jh["f_state"] = QString::number(ORDER_STATE_OPEN);
        jh["f_dateopen"] = current_date;
        jh["f_timeopen"] = current_time;
    } else {
        jh["f_id"] = jo.at(0).toObject()["f_id"].toString();
        C5WaiterOrderDoc w(jh["f_id"].toString(), srh.fDb);
        jh = w.fHeader;
        jb = w.fItems;
    }
}

void C5WaiterServer::saveOrder(QJsonObject &jh, QJsonArray &ja, C5Database &db)
{
    db.startTransaction();
    if (jh.contains("unlocktable")) {
        if (jh["unlocktable"].toString().toInt() > 0) {
            db[":f_lock"] = 0;
            db[":f_lockSrc"] = "";
            db[":f_id"] = jh["f_table"].toString();
            db.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
        }
    }
    if (jh["f_id"].toString().isEmpty() && ja.count() > 0) {
        db[":f_id"] = jh["f_hall"].toString().toInt();
        db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
        if (db.nextRow()) {
            jh["f_hallid"] = db.getString(0);
            jh["f_prefix"] = db.getString(1);
            db[":f_counter"] = db.getInt(0);
            db.update("h_halls", where_id(jh["f_hall"].toString()));
        } else {
            jh["f_hallid"] = "0";
        }
        jh["f_id"] = C5Database::uuid();
        db[":f_id"] = jh["f_id"].toString();
        db[":f_hallid"] = jh["f_hallid"].toString().toInt();
        db[":f_dateOpen"] = QDate::fromString(jh["f_dateopen"].toString(), FORMAT_DATE_TO_STR);
        db[":f_timeOpen"] = QTime::fromString(jh["f_timeopen"].toString(), FORMAT_TIME_TO_STR);
        db[":f_prefix"] = jh["f_prefix"].toString();
        db.insert("o_header", false);
    }
    if (jh["f_otherid"].toString().toInt() == PAYOTHER_TRANSFER_TO_ROOM) {
        db[":f_id"] = jh["f_id"].toString();
        db.exec("select * from o_pay_room where f_id=:f_id");
        if (db.nextRow()) {
            db[":f_res"] = jh["f_other_res"].toString();
            db[":f_inv"] = jh["f_other_inv"].toString();
            db[":f_room"] = jh["f_other_room"].toString();
            db[":f_guest"] = jh["f_other_guest"].toString();
            db.update("o_pay_room", where_id(jh["f_id"].toString()));
        } else {
            db[":f_id"] = jh["f_id"].toString();
            db[":f_res"] = jh["f_other_res"].toString();
            db[":f_inv"] = jh["f_other_inv"].toString();
            db[":f_room"] = jh["f_other_room"].toString();
            db[":f_guest"] = jh["f_other_guest"].toString();
            db.insert("o_pay_room", false);
        }
    } else {
        db[":f_id"] = jh["f_id"].toString();
        db.exec("delete from o_pay_room where f_id=:f_id");
    }
    if (jh["f_otherid"].toString().toInt() == PAYOTHER_CL) {
        db[":f_id"] = jh["f_id"].toString();
        db.exec("select * from o_pay_cl where f_id=:f_id");
        if (db.nextRow()) {
            db[":f_code"] = jh["f_other_clcode"].toString();
            db[":f_name"] = jh["f_other_clname"].toString();
            db.update("o_pay_cl", where_id(jh["f_id"].toString()));
        } else {
            db[":f_id"] = jh["f_id"].toString();
            db[":f_code"] = jh["f_other_clcode"].toString();
            db[":f_name"] = jh["f_other_clname"].toString();
            db.insert("o_pay_cl", false);
        }
    } else {
        db[":f_id"] = jh["f_id"].toString();
        db.exec("delete from o_pay_cl where f_id=:f_id");
    }
    if (jh.contains("f_bonusid")) {
        db[":f_data"] = jh["f_discountamount"].toString().toDouble();
        db.update("b_history", where_id(jh["f_bonusid"].toString().toInt()));
    }
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject jb = ja.at(i).toObject();
        saveDish(jh, jb, db);
        ja[i] = jb;
    }
    if (ja.count() > 0) {
        db[":f_comment"] = jh["f_comment"].toString();
        db[":f_staff"] = jh["f_staff"].toString().toInt();
        db[":f_hall"] = jh["f_hall"].toString().toInt();
        db[":f_table"] = jh["f_table"].toString().toInt();
        db[":f_state"] = jh["f_state"].toString().toInt();
        db[":f_amountTotal"] = jh["f_amounttotal"].toString().toDouble();
        db[":f_amountCash"] = jh["f_amountcash"].toString().toDouble();
        db[":f_amountCard"] = jh["f_amountcard"].toString().toDouble();
        db[":f_amountBank"] = jh["f_amountbank"].toString().toDouble();
        db[":f_amountOther"] = jh["f_amountother"].toString().toDouble();
        db[":f_servicemode"] = jh["f_servicemode"].toString().toInt();
        db[":f_amountService"] =jh["f_amountservice"].toString().toDouble();
        db[":f_amountDiscount"] = jh["f_amountdiscount"].toString().toDouble();
        db[":f_servicefactor"] = jh["f_servicefactor"].toString().toDouble();
        db[":f_discountfactor"] = jh["f_discountfactor"].toString().toDouble();
        db[":f_creditcardid"] = jh["f_creditcardid"].toString().toInt();
        db[":f_otherid"] = jh["f_otherid"].toString().toInt();
        db.update("o_header", where_id(jh["f_id"].toString()));
    }
    db.commit();
}

void C5WaiterServer::saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db)
{
    if (o["f_id"].toString().isEmpty()) {
        o["f_id"] = C5Database::uuid();
        db[":f_id"] = o["f_id"].toString();
        db.insert("o_body", false);
    }
    if (o["f_state"].toString().toInt() < 0) {
        o["f_state"] = QString::number(o["f_state"].toString().toInt() * -1);
        C5PrintRemovedServiceThread *pr = new C5PrintRemovedServiceThread(h, o);
        pr->start();
    }
    db[":f_header"] = h["f_id"].toString();
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
    db[":f_adgcode"] = o["f_adgcode"].toString();
    db[":f_removereason"] = o["f_removereason"].toString();
    db.update("o_body", where_id(o["f_id"].toString()));
}

int C5WaiterServer::printTax(const QJsonObject &h, const QJsonArray &ja, C5Database &db)
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
    double serviceTotal = 0;
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject d = ja[i].toObject();
        if (d["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        double price = d["f_price"].toString().toDouble();
        if (h["f_servicefactor"].toString().toDouble() > 0.001) {
            if (h["f_servicemode"].toString().toInt() == SERVICE_AMOUNT_MODE_INCREASE_PRICE) {
                price += price * h["f_servicefactor"].toString().toDouble();
            } else {
                serviceTotal += d["f_qty1"].toString().toDouble() * price * h["f_servicefactor"].toString().toDouble();
            }
        }
        pt.addGoods(C5Config::taxDept(), d["f_adgcode"].toString(), d["f_dish"].toString(), d["f_name"].toString(), price, d["f_qty2"].toString().toDouble(), 0);
    }
    if (serviceTotal > 0.001) {
        pt.addGoods(C5Config::taxDept(), "5901", "001", QString("%1 %2%").arg(tr("Service")).arg(float_str(h["f_servicefactor"].toString().toDouble()*100, 2)), serviceTotal, 1.0, 0);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(h["f_amountcard"].toString().toDouble(), 0, jsonIn, jsonOut, err);
    db[":f_order"] = h["f_id"].toString().toInt();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    if (result == pt_err_ok) {
        QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = h["f_id"].toString();
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = h["f_id"].toString();
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax", false);
    }
    return result;
}

bool C5WaiterServer::printBill(QJsonObject &jh, QJsonArray &jb, QString &err, C5ServerHandler &srh)
{
    saveOrder(jh, jb, srh.fDb);
    // CHECKING FOR RECEIPT PRINT COUNT
    if (jh["f_print"].toString().toInt() > 0) {
        if (!checkPermission(jh["f_currentstaff"].toString().toInt(), cp_t5_multiple_receipt, srh.fDb)) {
            err = tr("You cannot print more then 1 copies of receipt");
            return false;
        }
    }
    // CHECKING ALL ITEMS THAT WAS PRINTED
    for (int i = 0; i < jb.count(); i++) {
        QJsonObject jo = jb[i].toObject();
        if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
            err += tr("All service must be complited");
            break;
        }
    }
    if (jh["f_otherid"].toString().toInt() == PAYOTHER_SELFCOST) {
        C5WaiterOrderDoc w(jh, jb, srh.fDb);
        w.calculateSelfCost();
        jh = w.fHeader;
        jb = w.fItems;
    }
    // TODO: CHECK FOR DESTINATION PRINTER AND REDIRECT QUERY
    if (err.isEmpty()) {
        C5PrintReceiptThread *pr = new C5PrintReceiptThread(C5Config::dbParams(), jh, jb, C5Config::localReceiptPrinter());
        pr->fBill = true;
        pr->start();
    }
    return err.isEmpty();
}

bool C5WaiterServer::printReceipt(QJsonObject &jh, QJsonArray &jb, QString &err, C5ServerHandler &srh)
{
    saveOrder(jh, jb, srh.fDb);
    // CHECKING FOR RECEIPT PRINT COUNT
    if (jh["f_print"].toString().toInt() > 0) {
        if (!checkPermission(jh["f_currentstaff"].toString().toInt(), cp_t5_multiple_receipt, srh.fDb)) {
            err = tr("You cannot print more then 1 copies of receipt");
            return false;
        }
    }
    // CHECKING ALL ITEMS THAT WAS PRINTED
    for (int i = 0; i < jb.count(); i++) {
        QJsonObject jo = jb[i].toObject();
        if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
            err += tr("All service must be complited");
            break;
        }
    }
    // CHECKING TAX AND PRINT IF NEEDED
    QJsonArray jtax;
    QMap<QString, QVariant> bv;
    bv[":f_id"] = jh["f_id"].toString();
    srh.getJsonFromQuery("select * from o_tax where f_id=:f_id", jtax, bv);
    if (jtax.count() == 0 && jh["f_printtax"].toString().toInt()) {
        int result = printTax(jh, jb, srh.fDb);
        if (result != pt_err_ok) {
            err += tr("Print tax error");
        } else {
            bv[":f_id"] = jh["f_id"].toString();
            srh.getJsonFromQuery("select * from o_tax where f_id=:f_id", jtax, bv);
        }
    } else {

    }
    if (jh["f_otherid"].toString().toInt() == PAYOTHER_SELFCOST) {
        C5WaiterOrderDoc w(jh, jb, srh.fDb);
        w.calculateSelfCost();
        jh = w.fHeader;
        jb = w.fItems;
    }
    // CHECKING FOR TAX CASH/CARD
    // PRINT RECEIPT
    // TODO: CHECK FOR DESTINATION PRINTER AND REDIRECT QUERY
    if (err.isEmpty()) {
        if (jh["f_printtax"].toString().toInt()) {
            jh["f_dept"] = jtax[0].toObject()["f_dept"].toString();
            jh["f_firmname"] = jtax[0].toObject()["f_firmname"].toString();
            jh["f_address"] = jtax[0].toObject()["f_address"].toString();
            jh["f_devnum"] = jtax[0].toObject()["f_devnum"].toString();
            jh["f_serial"] = jtax[0].toObject()["f_serial"].toString();
            jh["f_fiscal"] = jtax[0].toObject()["f_fiscal"].toString();
            jh["f_receiptnumber"] = jtax[0].toObject()["f_receiptnumber"].toString();
            jh["f_hvhh"] = jtax[0].toObject()["f_hvhh"].toString();
            jh["f_fiscalmode"] = jtax[0].toObject()["f_fiscalmode"].toString();
            jh["f_taxtime"] = jtax[0].toObject()["f_time"].toString();
        }
        jh["f_print"] = QString::number(abs(jh["f_print"].toString().toInt()) + 1);
        srh.fDb[":f_print"] = jh["f_print"].toString().toInt();
        srh.fDb.update("o_header", where_id(jh["f_id"].toString()));
        jh["f_idramid"] = C5Config::idramID();
        jh["f_idramphone"] = C5Config::idramPhone();
        C5Database db(C5Config::dbParams());
        QString printerName = "local";
        db[":f_name"] = jh["receipt_printer"].toString();
        db.exec("select f_id from s_settings_names where lower(f_name)=lower(:f_name)");
        if (db.nextRow()) {
            db[":f_settings"] = db.getInt(0);
            db[":f_key"] = param_local_receipt_printer;
            db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
            if (db.nextRow()) {
                printerName = db.getString(0);
            }
        }
        C5PrintReceiptThread *pr = new C5PrintReceiptThread(C5Config::dbParams(), jh, jb, printerName);
        pr->start();
    }
    return err.isEmpty();
}

void C5WaiterServer::remember(const QJsonObject &h)
{
    C5Database db(C5Config::dbParams().at(0), C5Config::logDatabase(), C5Config::dbParams().at(2), C5Config::dbParams().at(3));
    db[":f_comp"] = h["comp"].toString();
    db[":f_date"] = QDate::fromString(h["date"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_time"] = QTime::fromString(h["time"].toString(), FORMAT_TIME_TO_STR);
    db[":f_user"] = h["user"].toString();
    db[":f_type"] = h["type"].toInt();
    db[":f_rec"] = h["rec"].toString();
    db[":f_invoice"] = h["invoice"].toString();
    db[":f_reservation"] = h["reservation"].toString();
    db[":f_action"] = h["action"].toString();
    db[":f_value1"] = h["value1"].toString();
    db[":f_value2"] = h["value2"].toString();
    db.insert("log", false);
}

void C5WaiterServer::getVersions(C5Database &db, QJsonArray &arr)
{
    db.exec("select * from s_app");
    while (db.nextRow()) {
        QJsonObject o;
        o["app"] = db.getString(0);
        o["version"] = db.getString(1);
        arr.append(o);
    }
}
