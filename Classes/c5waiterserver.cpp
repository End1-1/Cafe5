#include "c5waiterserver.h"
#include "c5config.h"
#include "c5socketmessage.h"
#include "c5printservicethread.h"
#include "c5printreceiptthread.h"
#include "c5printremovedservicethread.h"
#include "c5permissions.h"
#include "printtaxn.h"
#include "c5waiterorderdoc.h"
#include "c5storedraftwriter.h"
#include "c5printing.h"
#include "c5utils.h"
#include "datadriver.h"
#include "notificationwidget.h"
#include "c5jsondb.h"
#include "cashboxconfig.h"
#include "c5logsystem.h"
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
typedef void ( *caption)(QString &str);
typedef void ( *json)(C5Database &db, const QJsonObject &params, QJsonArray &jarr);
typedef void ( *translator)(QTranslator &trans);

C5WaiterServer::C5WaiterServer(QJsonObject &o, QTcpSocket *socket) :
    fIn(o)
{
    fSocket = socket;
    fPeerAddress = QHostAddress(fSocket->peerAddress().toIPv4Address()).toString();
}

void C5WaiterServer::reply(QJsonObject &o)
{
    C5ServerHandler srh;
#ifdef QT_DEBUG
    qDebug() << fIn;
#endif
    //C5Database().logEvent(QJsonDocument(fIn).toJson(QJsonDocument::Compact));
    QMap<QString, QVariant> bv;
    int cmd = fIn["cmd"].toInt();
    o["cmd"] = cmd;
    switch (cmd) {
        case sm_hall: {
            QJsonArray jHall;
            QString hallFilter = fIn["hall"].toString();
            srh.getJsonFromQuery(QString("select f_id, f_name, f_settings from h_halls %1")
                                 .arg(hallFilter.isEmpty() ? "" : " where f_id in (" + hallFilter + ")"), jHall);
            QJsonArray jTables;
            srh.getJsonFromQuery(QString("select t.f_id, t.f_hall, t.f_name, t.f_lock, t.f_lockSrc, \
             h.f_id as f_header, concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_staffName, \
                      h.f_amounttotal as f_amount, h.f_print, bc.f_govnumber, \
                      date_format(h.f_dateopen, '%d.%m.%Y') as f_dateopen, h.f_timeOpen, \
                      t.f_special_config \
                      from h_tables t \
                      left join o_header h on h.f_table=t.f_id and h.f_state=1 \
 left join o_header_options o on o.f_id=h.f_id \
 left join b_car bc on bc.f_id=o.f_car \
 left join s_user u on u.f_id=h.f_staff  %1 \
 order by f_id")
                                 .arg(hallFilter.isEmpty() ? "" : " where t.f_hall in (" + hallFilter + ") "), jTables);
            //srh.getJsonFromQuery("select f_id, f_hall, f_name, f_lock, f_lockSrc from h_tables order by f_id", jTables);
            QJsonArray jShift;
            srh.getJsonFromQuery("select f_id, f_name from s_salary_shift", jShift);
            o["reply"] = 1;
            o["halls"] = jHall;
            o["tables"] = jTables;
            o["shifts"] = jShift;
            break;
        }
        case sm_menu: {
            QJsonArray jMenu;
            QString query =
                "select d.f_id as f_dish, mn.f_name as menu_name, p1.f_name as part1, p2.f_name as part2, p2.f_adgCode, d.f_name, \
 m.f_price, m.f_store, m.f_print1, m.f_print2, d.f_remind, d.f_comment as f_description, \
 s.f_name as f_storename, d.f_color as dish_color, p2.f_color as type_color, 1 as f_timeorder, d.f_hourlypayment, \
 d.f_service as f_canservice, d.f_discount as f_candiscount, dsl.f_qty as f_stoplistqty \
 from d_menu m \
 left join d_menu_names mn on mn.f_id=m.f_menu \
 left join d_dish d on d.f_id=m.f_dish \
 left join d_part2 p2 on p2.f_id=d.f_part \
 left join d_part1 p1 on p1.f_id=p2.f_part \
 left join c_storages s on s.f_id=m.f_store \
 left join d_stoplist dsl on dsl.f_dish=d.f_id \
 where m.f_state=1 \
 order by d.f_queue, d.f_name ";
            srh.getJsonFromQuery(query, jMenu);
            QJsonArray jMenuNames;
            srh.getJsonFromQuery("select f_id, f_name from d_menu_names", jMenuNames);
            QJsonArray jDishSpecial;
            srh.getJsonFromQuery("select f_dish, f_comment from d_special", jDishSpecial);
            QJsonArray jPackages;
            srh.getJsonFromQuery("select f_id, f_name, f_price from d_package where f_enabled=1", jPackages);
            QJsonArray jPackagesList;
            srh.getJsonFromQuery("select p.f_package, p.f_dish, d.f_name, p.f_price, '' as part1, '' as part2, p2.f_adgCode, 1 as f_store, '' as f_print1, '' as f_print2, "
                                 "'' as f_remind, '' as f_description, '' as f_storename, -1 as dish_color, -1 as type_color, 1 as f_timeorder "
                                 "from d_package_list p "
                                 "left join d_package dp on dp.f_id=p.f_package "
                                 "left join d_dish d on d.f_id=p.f_dish "
                                 "left join d_part2 p2 on p2.f_id=d.f_part "
                                 "left join d_part1 p1 on p1.f_id=p2.f_part "
                                 "where dp.f_enabled=1 ", jPackagesList);
            QJsonArray jStopList;
            srh.getJsonFromQuery("select f_dish, f_qty from d_stoplist", jStopList);
            o["reply"] = 1;
            o["menu"] = jMenu;
            o["menunames"] = jMenuNames;
            o["dishspecial"] = jDishSpecial;
            o["packages"] = jPackages;
            o["packageslist"] = jPackagesList;
            o["stoplist"] = jStopList;
            srh.fDb.exec("select f_version from s_app where lower(f_app)='menu'");
            if (srh.fDb.nextRow()) {
                o["version"] = srh.fDb.getString(0);
            } else {
                o["version"] = "1";
            }
            break;
        }
        case sm_checkuser: {
            QJsonArray jUser;
            QString pass = fIn["pass"].toString();
            if (fIn.contains("md5")) {
                pass = password(pass);
            }
            bv[":f_altPassword"] = pass;
            bv[":f_state"] = 1;
            srh.getJsonFromQuery("select f_id, f_group, f_first, f_last from s_user where f_altPassword=:f_altPassword and f_state=:f_state",
                                 jUser, bv);
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
            srh.fDb[":f_name"] = fIn["settings_name"].toString();
            srh.fDb.exec("select * from s_settings_names where f_name=:f_name");
            if (srh.fDb.nextRow()) {
                int settingsId = srh.fDb.getInt("f_id");
                srh.fDb[":f_settings"] = settingsId;
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
                o["reply"] = 0;
                o["msg"] = tr("Invalid settings name");
            }
            QJsonArray jOther;
            srh.getJsonFromQuery("select f_settings, f_key, f_value from s_settings_values", jOther);
            o["reply"] = 1;
            o["conf"] = jConf;
            o["otherconf"] = jOther;
            break;
        }
        case sm_opentable: {
            QMutexLocker ml( &mutex);
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
            o = fIn;
            QJsonObject jh = fIn["header"].toObject();
            QJsonArray ja = fIn["body"].toArray();
            saveOrder(o, jh, ja, srh.fDb);
            o["header"] = jh;
            o["body"] = ja;
            o["reply"] = 1;
            break;
        }
        case sm_printservice: {
            o["reply"] = 1;
            o["order"] = fIn["order"];
            C5PrintServiceThread ps(fIn["order"].toString());
            ps.fReprintList = fIn["reprint"].toString();
            ps.fBooking = fIn["booking"].toInt() == 1;
            ps.fUseAliases = fIn["alias"].toInt() > 0;
            ps.run();
            break;
        }
        case sm_printreceipt: {
            QString err;
            bool r = printReceipt(err, srh.fDb, false, fIn["alias"].toInt() > 0);
            o["reply"] = r ? 1 : 0;
            o["msg"] = err;
            o["close"] = fIn["close"];
            o["order"] = fIn["order"];
            break;
        }
        case sm_bill: {
            QString err;
            printReceipt(err, srh.fDb, true, fIn["alias"].toInt() > 0);
            o["reply"] = err.isEmpty() ? 1 : 0;
            o["msg"] = err;
            o["close"] = fIn["close"];
            o["order"] = fIn["order"];
            break;
        }
        case sm_closeorder: {
            processCloseOrder(o, srh.fDb);
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
            bv[":f_id"] = fIn["order"].toString();
            srh.getJsonFromQuery("select * from b_history where f_id=:f_id", ja, bv);
            if (ja.count() > 0) {
                o["reply"] = 0;
                o["msg"] = tr("Bonus system alreay exists for this order");
                break;
            }
            QJsonObject jo;
            //Check discount type
            bv[":f_code"] = fIn["code"].toString();
            srh.getJsonFromQuery("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
                                 "from b_cards_discount c "
                                 "left join c_partners p on p.f_id=c.f_client "
                                 "left join b_card_types cn on cn.f_id=c.f_mode "
                                 "where f_code=:f_code", ja, bv);
            if (ja.count() > 0) {
                o["reply"] = 1;
                srh.fDb[":f_type"] = ja.at(0).toObject()["f_value"].toString().toInt();
                srh.fDb[":f_value"] = ja.at(0).toObject()["f_value"].toString().toDouble();
                srh.fDb[":f_card"] = ja.at(0).toObject()["f_id"].toString().toInt();
                srh.fDb[":f_data"] = 0;
                srh.fDb[":f_id"] = fIn["order"].toString();
                srh.fDb.insert("b_history");
                jo["f_cardid"] = ja.at(0).toObject()["f_id"].toString();
                jo["f_cardtype"] = ja.at(0).toObject()["f_mode"].toString();
                jo["f_cardvalue"] = ja.at(0).toObject()["f_value"].toString();
                jo["f_cardname"] = ja.at(0).toObject()["f_name"].toString();
                jo["f_cardholder"] = ja.at(0).toObject()["f_contact"].toString();
                o["card"] = jo;
                break;
            } else {
            }
            if (fIn["handlevisit"].toInt() == 1) {
                o["handlevisit"] = 1;
                o["code"] = fIn["code"].toString();
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
            QDate d1 = QDate::fromString(fIn["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
            QDate d2 = QDate::fromString(fIn["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
            bv[":f_datecash1"] = d1;
            bv[":f_datecash2"] = d2;
            bv[":f_state"] = ORDER_STATE_CLOSE;
            QString sqlQuery;
            sqlQuery =
                "select oh.f_id, concat(oh.f_prefix, oh.f_hallid) as f_order, date_format(oh.f_datecash, '%d.%m.%Y') as f_datecash, oh.f_timeclose, "
                "h.f_name as f_hall, t.f_name as f_table, concat(u.f_last, ' ', u.f_first) as f_staff,"
                "oh.f_amounttotal, ot.f_receiptnumber "
                "from o_header oh "
                "left join h_halls h on h.f_id=oh.f_hall "
                "left join h_tables t on t.f_id=oh.f_table "
                "left join s_user u on u.f_id=oh.f_staff "
                "left join o_tax ot on ot.f_id=oh.f_id "
                "where (oh.f_state=:f_state and oh.f_datecash between :f_datecash1 and :f_datecash2) ";
            if (fIn["opened"].toString().toInt() > 0) {
                sqlQuery += " or (oh.f_state=1) ";
            }
            if (fIn["hall"].toString().toInt() > 0) {
                sqlQuery += QString(" and oh.f_hall=%1 ").arg(fIn["hall"].toString());
            }
            if (fIn["shift"].toString().toInt() > 0) {
                sqlQuery += QString(" and oh.f_shift=%1 ").arg(fIn["shift"].toString());
            }
            sqlQuery += "order by oh.f_timeclose";
            srh.getJsonFromQuery(sqlQuery, ja, bv);
            o["reply"] = 0;
            o["report"] = ja;
            break;
        }
        case sm_log:
            remember(fIn);
            break;
        case sm_apporder:
            processAppOrder(o);
            break;
        case sm_callstaff:
            processCallStaff(o);
            break;
        case sm_messagelist:
            processMessageList(o);
            break;
        case sm_callreceipt:
            processCallReceipt(o);
            break;
        case sm_getcostumer_by_car:
            processGetCostumerByCar(o);
            break;
        case sm_rotate_shift:
            processRotateShift(o);
            break;
        case sm_checkdiscount_by_visit:
            processCheckDiscountByVisit(o);
            break;
        case sm_print_removed_service:
            processPrintRemovedService(o);
            break;
        case sm_retrun_tax_receipt:
            processReturnTaxReceipt(o);
            break;
        case sm_tax_report:
            processTaxReport(o);
            break;
        default:
            o["reply"] = 0;
            o["msg"] = QString("%1: %2").arg(tr("Unknown command for socket handler from dlgface")).arg(cmd);
            break;
    }
}

int C5WaiterServer::cmd()
{
    return fIn["cmd"].toInt();
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
    if (fIn["orderid"].toString().isEmpty()) {
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
    } else {
        jh["f_id"] = jo.at(0).toObject()["f_id"].toString();
        C5WaiterOrderDoc w(jh["f_id"].toString(), srh.fDb);
        jh = w.fHeader;
        jb = w.fItems;
    }
}

void C5WaiterServer::saveOrder(QJsonObject &o, QJsonObject &jh, QJsonArray &ja, C5Database &db)
{
    db.startTransaction();
    if (jh.contains("unlocktable")) {
        if (jh["unlocktable"].toString().toInt() > 0) {
            db[":f_lock"] = 0;
            db[":f_lockSrc"] = "";
            db[":f_id"] = jh["f_table"].toString();
            db.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
        }
        if (jh["closeempty"].toString().toInt() == 1) {
            bool e = true;
            for (int i = 0; i < ja.count(); i++) {
                QJsonObject jb = ja.at(i).toObject();
                if (jb["f_state"].toString().toInt() == DISH_STATE_OK) {
                    e = false;
                    break;
                }
            }
            if (e) {
                jh["f_state"] = QString::number(ORDER_STATE_EMPTY);
                fIn["header"] = jh;
                processCloseOrder(o, db);
            }
        }
    }
    if (jh["f_id"].toString().isEmpty() && jh["f_table"].toString().toInt() > 0) {
        db[":f_id"] = jh["f_hall"].toString().toInt();
        db.exec("select f_counterhall from h_halls where f_id=:f_id");
        if (db.nextRow()) {
            int hallid = db.getInt(0);
            db[":f_id"] = hallid;
            db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
            if (db.nextRow()) {
                jh["f_hallid"] = db.getInt(0);
                jh["f_prefix"] = db.getString(1);
                db[":f_counter"] = db.getInt(0);
                db.update("h_halls", where_id(hallid));
            }
        } else {
            jh["f_hallid"] = 0;
        }
        jh["f_id"] = C5Database::uuid();
        db[":f_id"] = jh["f_id"].toString();
        db[":f_hallid"] = jh["f_hallid"].toInt();
        db[":f_dateopen"] = QDate::fromString(jh["f_dateopen"].toString(), FORMAT_DATE_TO_STR);
        db[":f_timeopen"] = QTime::fromString(jh["f_timeopen"].toString(), FORMAT_TIME_TO_STR);
        db[":f_prefix"] = jh["f_prefix"].toString();
        db.insert("o_header", false);
        db[":f_id"] = jh["f_id"].toString();
        db[":f_guests"] = jh["f_guests"].toString().toInt();
        db.insert("o_header_options", false);
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
    if (jh["f_table"].toString().toInt() > 0) {
        db[":f_comment"] = jh["f_comment"].toString();
        db[":f_staff"] = jh["f_staff"].toString().toInt();
        db[":f_hall"] = jh["f_hall"].toString().toInt();
        db[":f_table"] = jh["f_table"].toString().toInt();
        db[":f_state"] = jh["f_state"].toString().toInt();
        db[":f_amounttotal"] = jh["f_amounttotal"].toString().toDouble();
        db[":f_amountcash"] = jh["f_amountcash"].toString().toDouble();
        db[":f_amountcard"] = jh["f_amountcard"].toString().toDouble();
        db[":f_amountbank"] = jh["f_amountbank"].toString().toDouble();
        db[":f_amountother"] = jh["f_amountother"].toString().toDouble();
        db[":f_servicemode"] = jh["f_servicemode"].toString().toInt();
        db[":f_amountservice"] = jh["f_amountservice"].toString().toDouble();
        db[":f_amountdiscount"] = jh["f_amountdiscount"].toString().toDouble();
        db[":f_servicefactor"] = jh["f_servicefactor"].toString().toDouble();
        db[":f_discountfactor"] = jh["f_discountfactor"].toString().toDouble();
        db[":f_creditcardid"] = jh["f_creditcardid"].toString().toInt();
        db[":f_otherid"] = jh["f_otherid"].toString().toInt();
        db.update("o_header", where_id(jh["f_id"].toString()));
        db[":f_splitted"] = jh["f_splitted"].toString().toInt();
        db[":f_guests"] = jh["f_guests"].toString().toInt();
        db[":f_car"] = jh["car"].toString().toInt();
        db.update("o_header_options", where_id(jh["f_id"].toString()));
    }
    if (jh["preorder"].toString().toInt() == 1) {
        db[":f_id"] = jh["f_id"].toString();
        db.exec("select * from o_preorder where f_id=:f_id");
        db[":f_datefor"] = QDate::fromString(jh["date"].toString(), FORMAT_DATETIME_TO_STR);
        db[":f_timefor"] = QTime::fromString(jh["time"].toString(), FORMAT_TIME_TO_STR);
        db[":f_guests"] = jh["guest"].toString().toInt();
        db[":f_prepaidcash"] = jh["cash"].toString().toDouble();
        db[":f_prepaidcard"] = jh["card"].toString().toDouble();
        if (db.nextRow()) {
            db.update("o_preorder", "f_id", jh["f_id"].toString());
        } else {
            db[":f_id"] = jh["f_id"].toString();
            db.insert("o_preorder", false);
        }
        db[":f_state"] = ja.count() == 0 ? ORDER_STATE_PREORDER_EMPTY : ORDER_STATE_PREORDER_WITH_ORDER;
        db.update("o_header", "f_id", jh["f_id"].toString());
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
    db[":f_timeorder"] = o["f_timeorder"].toString().toInt();
    db[":f_package"] = o["f_package"].toString().toInt();
    db[":f_candiscount"] = o["f_candiscount"].toString().toInt();
    db[":f_canservice"] = o["f_canservice"].toString().toInt();
    db[":f_guest"] = o["f_guest"].toString().toInt();
    db[":f_row"] = o["f_row"].toString().toInt();
    db.update("o_body", where_id(o["f_id"].toString()));
}

void C5WaiterServer::processCloseOrder(QJsonObject &o, C5Database &db)
{
    QString err;
    QJsonArray jb = fIn["body"].toArray();
    QJsonObject jh = fIn["header"].toObject();
    int orderstate = jh["f_state"].toString().toInt();
    if (orderstate == ORDER_STATE_OPEN) {
        orderstate = ORDER_STATE_CLOSE;
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
    if (jh["f_print"].toString().toInt() < 1) {
        err += tr("Receipt was not printed");
    }
    // Get settgins
    QMap<int, QString> settings;
    db[":f_id"] = jh["f_hall"].toString().toInt();
    db.exec("select f_settings from h_halls where f_id=:f_id");
    if (!db.nextRow()) {
        o["reply"] = 0;
        o["msg"] = QString("%1. %2: %3").arg(tr("Get settings failed")).arg("Settings id").arg(jh["f_hall"].toString());
        return;
    }
    int settings_id = db.getInt(0);
    db[":f_id"] = jh["f_table"].toString().toInt();
    db.exec("select f_special_config from h_tables where f_id=:f_id");
    db.nextRow();
    if (db.getInt(0) > 0) {
        settings_id = db.getInt(0);
    }
    db[":f_settings"] = settings_id;
    db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
    while (db.nextRow()) {
        settings[db.getInt(0)] = db.getString(1);
    }
    QDate dateCash = QDate::currentDate();
    int dateShift = settings[param_date_cash_shift].toInt();
    bool isAuto = settings[param_date_cash_auto].toInt() > 0;
    if (isAuto) {
        QTime t = QTime::fromString(settings[param_working_date_change_time], "HH:mm:ss");
        if (t.isValid()) {
            if (QTime::currentTime() < t) {
                dateCash = dateCash.addDays(-1);
            }
        }
    } else {
        dateCash = QDate::fromString(settings[param_date_cash], FORMAT_DATE_TO_STR_MYSQL);
        if (!dateCash.isValid()) {
            o["reply"] = 0;
            o["msg"] = tr("Date cash set manual and it is not valid.");
            return;
        }
    }
    if (err.isEmpty()) {
        jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
        jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR);
        jh["f_datecash"] = dateCash.toString(FORMAT_DATE_TO_STR);
        db[":f_state"] = orderstate;
        db[":f_dateclose"] = QDate::currentDate();
        db[":f_datecash"] = QDate::fromString(jh["f_datecash"].toString(), FORMAT_DATE_TO_STR);
        db[":f_timeclose"] = QTime::fromString(jh["f_timeclose"].toString(), FORMAT_TIME_TO_STR);
        db[":f_staff"] = jh["f_currentstaff"].toString().toInt();
        db.update("o_header", where_id(jh["f_id"].toString()));
        db[":f_lock"] = 0;
        db[":f_lockSrc"] = "";
        db.update("h_tables", where_id(jh["f_table"].toString().toInt()));
        if (dateShift == 0) {
            db[":f_time"] = QTime::fromString(jh["f_timeclose"].toString(), FORMAT_TIME_TO_STR);
            db.exec("select f_shift from s_salary_shift_time where :f_time between f_start and f_end");
            if (db.nextRow()) {
                dateShift = db.getInt("f_shift");
            } else {
                dateShift = 1;
            }
        }
        db[":f_id"] = jh["f_id"].toString();
        db[":f_shift"] = dateShift;
        db.exec("update o_header set f_shift=:f_shift where f_id=:f_id");
        if (jh["f_otherid"].toString().toInt() == PAYOTHER_DEBT) {
            db[":f_order"] = jh["f_id"].toString();
            db.exec("select f_costumer, f_govnumber from o_header_options co "
                    "inner join b_car c on c.f_id=co.f_car "
                    "where f_order=:f_order");
            if (db.nextRow()) {
                db[":f_costumer"] = db.getInt(0);
                db[":f_order"] = jh["f_id"].toString();
                db[":f_amount"] = jh["f_amountother"].toString().toDouble() * -1;
                db[":f_date"] = dateCash;
                db[":f_govnumber"] = db.getString(1);
                db.insert("b_clients_debts", false);
            }
        }
        if (orderstate == ORDER_STATE_CLOSE) {
            if (jh["f_bonusid"].toString().toInt() > 0) {
                db[":f_id"] = jh["f_id"].toString();
                qDebug() << jh;
                switch (jh["f_bonustype"].toString().toInt()) {
                    case CARD_TYPE_DISCOUNT:
                        db[":f_data"] = jh["f_amountdiscount"].toString().toDouble();
                        break;
                    case CARD_TYPE_ACCUMULATIVE:
                        db[":f_data"] = jh["f_amounttotal"].toString().toDouble() * (jh["f_bonusvalue"].toString().toDouble() / 100);
                        break;
                }
                db.exec("update b_history set f_data=:f_data where f_id=:f_id");
            }
            C5WaiterOrderDoc w(db, jh, jb);
            w.transferToHotel(db, err);
            if (settings[param_waiter_automatially_storeout].toInt() > 0) {
                w.makeOutputOfStore(db, err, DOC_STATE_SAVED);
            }
            C5StoreDraftWriter dw(db);
            auto *cbc = Configs::construct<CashboxConfig>(db.dbParams(), 2);
            if (settings[param_autoinput_salecash].toInt() == 1) {
                QString headerPrefix;
                int headerId;
                if (!dw.hallId(headerPrefix, headerId, jh["f_hall"].toString().toInt())) {
                    err = dw.fErrorMsg;
                }
                if (jh["f_amountcash"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountcash"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash1, dateCash);
                }
                if (jh["f_amountcard"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountcard"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash2, dateCash);
                }
                if (jh["f_amountbank"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountbank"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash2, dateCash);
                }
                if (jh["f_amountprepaid"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountprepaid"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash2, dateCash);
                }
                if (jh["f_amountidram"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountidram"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash2, dateCash);
                }
                if (jh["f_amountpayx"].toString().toDouble() > 0.0001) {
                    writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
                                 jh["f_amountpayx"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
                                 cbc->cash2, dateCash);
                }
            }
        } else {
            C5WaiterOrderDoc::removeDocument(db, jh["f_id"].toString());
        }
    }
    o["reply"] = err.isEmpty() ? 1 : 0;
    o["msg"] = err;
}

int C5WaiterServer::printTax(const QMap<QString, QVariant> &header, const QList<QMap<QString, QVariant> > &body,
                             QString &err)
{
    QElapsedTimer et;
    et.start();
    C5Database db(__c5config.dbParams());
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(),  this);
    for (int i = 0; i < body.count(); i++) {
        const QMap<QString, QVariant> &m = body.at(i);
        if (m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        pt.addGoods(C5Config::taxDept().toInt(),
                    dbdishpart2->adgcode(dbdish->part2(m["f_dish"].toInt())),
                    m["f_dish"].toString(),
                    dbdish->name(m["f_dish"].toInt()),
                    m["f_price"].toDouble(),
                    m["f_qty2"].toDouble(),
                    m["f_discount"].toDouble() * 100);
    }
    if (header["f_amountservice"].toDouble() > 0.001) {
        pt.addGoods(C5Config::taxDept().toInt(),
                    "5901",
                    "001",
                    QString("%1 %2%").arg(tr("Service")).arg(float_str(header["f_servicefactor"].toDouble() * 100, 2)),
                    header["f_amountservice"].toDouble(), 1.0, 0);
    }
    QString jsonIn, jsonOut;
    int result = 0;
    double cardamount = header["f_amountcard"].toDouble()
                        + header["f_amountidram"].toDouble()
                        + header["f_amountpayx"].toDouble();
    result = pt.makeJsonAndPrint(cardamount, header["f_amountprepaid"].toDouble(), jsonIn, jsonOut, err);
    QJsonObject jtax;
    QJsonParseError jsonErr;
    jtax["f_order"] = header["f_id"].toString();
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(jsonIn.toUtf8(), &jsonErr).object();
    jtax["f_out"] = QJsonDocument::fromJson(jsonOut.toUtf8()).object();;
    jtax["f_err"] = err;
    jtax["f_result"] = result;
    jtax["f_state"] = result == pt_err_ok ? 1 : 0;
    if (jsonErr.error != QJsonParseError::NoError) {
    }
    if (!db.exec(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(
                     QJsonDocument::Compact))))) {
    }
    return result;
}

bool C5WaiterServer::printReceipt(QString &err, C5Database &db, bool isBill, bool alias)
{
    if (fIn["close"].toInt() > 0) {
        db[":f_id"] = fIn["order"].toString();
        db.exec("select f_otherid from o_header where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt("f_otherid") == PAYOTHER_PRIMECOST) {
                C5WaiterOrderDoc w(fIn["order"].toString(), db);
                w.calculateSelfCost(db);
            }
        }
    }
    QMap<QString, QVariant> headerInfo;
    db[":f_id"] = fIn["order"].toString();
    if (!db.exec("select o.*, concat_ws(' ', u.f_last, u.f_first) as f_currentstaffname,"
                 "t.f_name as f_tablename, h.f_name as f_hallname "
                 "from o_header o "
                 "left join h_tables t on t.f_id=o.f_table "
                 "left join h_halls h on h.f_id=o.f_hall "
                 "left join s_user u on u.f_id=o.f_currentstaff "
                 "where o.f_id=:f_id")) {
        err = db.fLastError;
        return false;
    }
    if (!db.nextRow()) {
        err = tr("No order with this id");
        return false;
    }
    db.rowToMap(headerInfo);
    QList<QMap<QString, QVariant> > bodyInfo;
    db[":f_header"] = fIn["order"].toString();
    if (!db.exec("select f_state, f_dish, f_price, f_canservice, f_candiscount, ob.f_discount, "
                 "d.f_name as f_dishname, d.f_adgt, d.f_hourlypayment, d.f_extra, "
                 "sum(f_qty1) as f_qty1, sum(f_qty2) as f_qty2, "
                 "sum(f_total) as f_total, f_adgcode, ob.f_comment "
                 "from o_body ob "
                 "left join d_dish d on d.f_id=ob.f_dish "
                 "where f_header=:f_header "
                 "group by f_dish, f_state, f_price, ob.f_discount "
                 "order by ob.f_row ")) {
        err = db.fLastError;
        return false;
    }
    while (db.nextRow()) {
        QMap<QString, QVariant> m;
        db.rowToMap(m);
        bodyInfo.append(m);
    }
    if (fIn["printtax"].toInt() == 1 ) {
        bool printtax = false;
        db[":f_id"] = fIn["order"].toString();
        db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt(0) == 0) {
                printtax = true;
            }
        } else {
            printtax = true;
        }
        if (printtax) {
            if (printTax(headerInfo, bodyInfo, err) != pt_err_ok) {
                return false;
            }
        }
    }
#ifdef QT_DEBUG
    qDebug() << "Error after printtax" << err;
    err.clear();
#endif
    // CHECKING FOR RECEIPT PRINT COUNT
    //    if (jh["f_print"].toString().toInt() > 0) {
    //        if (!checkPermission(jh["f_currentstaff"].toString().toInt(), cp_t5_multiple_receipt, srh.fDb)) {
    //            err = tr("You cannot print more then 1 copies of receipt");
    //            return false;
    //        }
    //    }
    // CHECKING ALL ITEMS THAT WAS PRINTED
    //    for (int i = 0; i < jb.count(); i++) {
    //        QJsonObject jo = jb[i].toObject();
    //        if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
    //            continue;
    //        }
    //        if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
    //            err += tr("All service must be complited");
    //            break;
    //        }
    //    }
    //CHECK SELFCOST
    // CHECKING FOR TAX CASH/CARD
    // PRINT RECEIPT
    // TODO: CHECK FOR DESTINATION PRINTER AND REDIRECT QUERY//        jh["f_idramid"] = C5Config::idramID();
    //        jh["f_idramphone"] = C5Config::idramPhone();
    QString printerName = "local";
    int paperWidth = 500;
    C5LogSystem::writeEvent("Settings name: " + fIn["receipt_printer"].toString());
    db[":f_name"] = fIn["receipt_printer"].toString();
    db.exec("select f_id from s_settings_names where f_name=:f_name");
    if (db.nextRow()) {
        int s = db.getInt("f_id");
        db[":f_settings"] = s;
        db[":f_key"] = param_local_receipt_printer;
        db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
        if (db.nextRow()) {
            printerName = db.getString(0);
        }
        db[":f_settings"] = s;
        db[":f_key"] = param_print_paper_width;
        db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
        if (db.nextRow()) {
            paperWidth = db.getString(0).toInt() == 0 ? 650 : db.getString(0).toInt();
            C5LogSystem::writeEvent(QString("Paper width was changed: %1").arg(paperWidth));
        }
    }
    if (alias) {
        db[":f_alias"] = printerName;
        db.exec("select * from d_print_aliases where f_alias=:f_alias");
        if (db.nextRow()) {
            printerName = db.getString("f_printer");
        }
    }
    if (fIn["withoutprint"].toInt() == 0 && fIn["nofinalreceipt"].toInt() == 0) {
        C5PrintReceiptThread pr(fIn["order"].toString(), headerInfo, bodyInfo,
                                printerName, fIn["language"].toInt(), paperWidth);
        pr.fBill = isBill;
        pr.fIdram[param_idram_name] = C5Config::getValue(param_idram_name);
        pr.fIdram[param_idram_id] = C5Config::getValue(param_idram_id);
        pr.fIdram[param_idram_phone] = C5Config::getValue(param_idram_phone);
        pr.fIdram[param_idram_tips] = C5Config::getValue(param_idram_tips);
        pr.fIdram[param_idram_tips_wallet] = C5Config::getValue(param_idram_tips_wallet);
        if (!pr.print(__c5config.dbParams())) {
            err = pr.fError;
        }
    }
    if (err.isEmpty()) {
        if (isBill) {
            db[":f_id"] = fIn["order"].toString();
            db[":f_precheck"] = abs(headerInfo["f_precheck"].toInt()) + 1;
            db.exec("update o_header set f_precheck=:f_precheck where f_id=:f_id");
        } else {
            db[":f_id"] = fIn["order"].toString();
            db[":f_print"] = abs(headerInfo["f_print"].toInt()) + 1;
            db.exec("update o_header set f_print=:f_print where f_id=:f_id");
        }
    }
    return err.isEmpty();
}

void C5WaiterServer::remember(const QJsonObject &h)
{
    C5Database db(C5Config::dbParams().at(0), C5Config::logDatabase(), C5Config::dbParams().at(2),
                  C5Config::dbParams().at(3));
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
    db.insert("airlog.log", false);
}

void C5WaiterServer::processAppOrder(QJsonObject &o)
{
    QString orderinfo;
    C5Database db(C5Config::dbParams());
    QString id = fIn["header"].toString();
    if (id.isEmpty()) {
        id = db.uuid();
        db[":f_id"] = fIn["hallid"].toString().toInt();
        db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
        if (db.nextRow()) {
            o["f_hallid"] = db.getInt(0);
            o["f_prefix"] = db.getString(1);
            db[":f_counter"] = db.getInt(0);
            db.update("h_halls", where_id(fIn["hallid"].toString()));
        } else {
            o["f_hallid"] = 0;
            o["f_prefix"] = "";
        }
        db[":f_id"] = id;
        db[":f_hallid"] = o["f_hallid"].toInt();
        db[":f_prefix"] = o["f_prefix"].toString();
        db[":f_state"] = ORDER_STATE_OPEN;
        db[":f_hall"] = fIn["hallid"].toString();
        db[":f_table"] = fIn["tableid"].toString();
        db[":f_dateopen"] = QDate::currentDate();
        db[":f_timeopen"] = QTime::currentTime();
        db[":f_staff"] = fIn["staffid"].toString();
        db[":f_comment"] = "";
        db[":f_print"] = 0;
        db[":f_amounttotal"] = 0;
        db[":f_amountcash"] = 0;
        db[":f_amountcard"] = 0;
        db[":f_amountbank"] = 0;
        db[":f_amountother"] = 0;
        db[":f_amountservice"] = 0;
        db[":f_amountservice"] = 0;
        db[":f_amountdiscount"] = 0;
        db[":f_servicefactor"] = fIn["servicevalue"].toString().toDouble();
        db[":f_discountfactor"] = 0;
        db[":f_creditcardid"] = 0;
        db[":f_otherid"] = 0;
        db.insert("o_header", false);
    }
    orderinfo += QString("%1 %2, %3<br>").arg(tr("Order")).arg(fIn["tablename"].toString()).arg(
                     o["f_hallprefix"].toString() + QString::number(o["f_hallid"].toInt()));
    QJsonArray jd = fIn["dishes"].toArray();
    QJsonArray jout;
    QJsonArray jprint;
    for (int i = 0; i < jd.count(); i++) {
        QJsonObject d = jd.at(i).toObject();
        if (d["remoteid"].toString().isEmpty()) {
            d["remoteid"] = C5Database::uuid();
            db[":f_id"] = d["remoteid"].toString();
            db.insert("o_body", false);
        }
        if (d["qty2"].toString().toDouble() < d["qty1"].toString().toDouble()) {
            QJsonObject jdp;
            jdp["f_qtyprint"] = QString::number(d["qty1"].toString().toDouble() - d["qty2"].toString().toDouble(), 'f', 1);
            jdp["f_name"] = d["dishname"].toString();
            jdp["f_storename"] = d["store"].toString();
            jdp["f_print1"] = d["print1"].toString();
            jdp["f_print2"] = d["print2"].toString();
            jprint.append(jdp);
            d["qty2"] = d["qty1"].toString();
            orderinfo += QString("%1: %2<br>").arg(jdp["f_name"].toString()).arg(jdp["f_qtyprint"].toString());
        }
        db[":f_header"] = id;
        db[":f_state"] = d["state"].toString();
        db[":f_dish"] = d["dishcode"].toString().toInt();
        db[":f_qty1"] = d["qty1"].toString().toDouble();
        db[":f_qty2"] = d["qty2"].toString().toDouble();
        db[":f_price"] = d["price"].toString().toDouble();
        db[":f_service"] = fIn["servicevalue"].toString().toDouble();
        db[":f_discount"] = 0;
        db[":f_total"] = (d["qty2"].toString().toDouble() * d["price"].toString().toDouble()) +
                         (d["qty2"].toString().toDouble() * d["price"].toString().toDouble() * fIn["servicevalue"].toString().toDouble());
        db[":f_store"] = d["store"].toString().toInt();
        db[":f_print1"] = d["print1"].toString();
        db[":f_print2"] = d["print2"].toString();
        db[":f_comment"] = "";
        db[":f_remind"] = d["remind"].toString().toInt();
        db[":f_adgcode"] = d["adgcode"].toString();
        db[":f_removereason"] = "";
        db.update("o_body", where_id(d["remoteid"].toString()));
        jout.append(d);
    }
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    o["header"] = C5JsonDb::convertRowToJsonObject(db);
    o["body"] = jout;
    o["reply"] = 1;
    if (jprint.count() > 0) {
        o["msg"] = tr("Your order was accepted");
        QJsonObject jhprint;
        jhprint["f_tablename"] = fIn["tablename"].toString();
        jhprint["f_prefix"] = o["header"].toObject()["f_prefix"].toString();
        jhprint["f_hallid"] = o["header"].toObject()["f_hallid"].toInt();
        jhprint["f_currentstaffname"] = fIn["staffname"].toString();
        C5PrintServiceThread ps(id);
        ps.run();
        NotificationWidget::showMessage(orderinfo);
    } else {
        o["msg"] = tr("Your order is empty or already was accepted");
        NotificationWidget::showMessage(tr("The customer was make an empty order, can you help him?"));
    }
}

void C5WaiterServer::processCallStaff(QJsonObject &o)
{
    QFont font(qApp->font());
    font.setPointSize(30);
    C5Printing p;
    p.setSceneParams(650, 28000, QPrinter::Portrait);
    p.setFont(font);
    p.ltext(tr("Table"), 0);
    p.rtext(fIn["table"].toString());
    p.br();
    p.ltext(tr("Call staff"), 0);
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    o["reply"] = 1;
    NotificationWidget::showMessage(QString("%1 %2: %3").arg(tr("Table")).arg(fIn["table"].toString()).arg(
                                        tr("Call staff")));
}

void C5WaiterServer::processMessageList(QJsonObject &o)
{
    C5Database db(C5Config::dbParams());
    db[":f_id"] = fIn["lastmessage"].toInt();
    db.exec("select f_id, msg from droid_message where f_id>:f_id");
    o["list"] = C5JsonDb::convertRowsToJsonArray(db);
    o["reply"] = 1;
}

void C5WaiterServer::processCallReceipt(QJsonObject &o)
{
    QFont font(qApp->font());
    font.setPointSize(30);
    C5Printing p;
    p.setSceneParams(650, 28000, QPrinter::Portrait);
    p.setFont(font);
    p.ltext(tr("Table"), 0);
    p.rtext(fIn["tablename"].toString());
    p.br();
    p.ltext(tr("Call receipt"), 0);
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    o["reply"] = 1;
    NotificationWidget::showMessage(QString("%1 %2: %3").arg(tr("Table")).arg(fIn["tablename"].toString()).arg(
                                        tr("Call receipt")));
}

void C5WaiterServer::processGetCostumerByCar(QJsonObject &o)
{
    C5Database db(C5Config::dbParams());
    db[":f_id"] = fIn["car"].toString().toInt();
    db.exec("select bc.f_id, concat(c.f_name, ', ', bc.f_govnumber, ', ', trim(cl.f_contact)) as f_name "
            "from b_car bc "
            "left join c_partners cl on cl.f_id=bc.f_costumer "
            "left join s_car c on c.f_id=bc.f_car "
            "where bc.f_id=:f_id");
    if (db.nextRow()) {
        o["name"] = db.getString(1);
        o["id"] = db.getString(0);
        o["reply"] = 1;
    } else {
        o["reply"] = 0;
    }
}

void C5WaiterServer::processRotateShift(QJsonObject &o)
{
    C5Database db(C5Config::dbParams());
    db[":f_value"] = fIn["date"].toString();
    db[":f_key"] = param_date_cash;
    db.exec("update s_settings_values set f_value=:f_value where f_key=:f_key");
    db[":f_value"] = fIn["shift"].toString();
    db[":f_key"] = param_date_cash_shift;
    db.exec("update s_settings_values set f_value=:f_value where f_key=:f_key");
    o["reply"] = 1;
}

void C5WaiterServer::processCheckDiscountByVisit(QJsonObject &o)
{
    C5Database db(C5Config::dbParams());
    db[":f_govnumber"] = fIn["code"].toString();
    db.exec("select count(f_order), f_costumer from o_header_options bco "
            "inner join b_car bc on bc.f_id=bco.f_car "
            "where bc.f_govnumber=:f_govnumber");
    if (db.nextRow()) {
        o["visit"] = db.getInt(0);
        o["client"] = db.getInt(1);
    } else {
        o["reply"] = 0;
        o["msg"] = tr("Invalide card code");
        return;
    }
    db[":f_code"] = "auto_" + fIn["code"].toString();
    db[":f_mode"] = CARD_TYPE_AUTO_DISCOUNT;
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (db.nextRow()) {
        o["card_code"] = db.getString("f_code");
    } else {
        o["card_code"] = "auto_" + fIn["code"].toString();
        db[":f_mode"] = CARD_TYPE_AUTO_DISCOUNT;
        db[":f_code"] = "auto_" + fIn["code"].toString();
        db[":f_client"] = o["client"].toInt();
        db[":f_value"] = 100;
        db[":f_datestart"] = QDate::currentDate();
        db[":f_dateend"] = QDate::currentDate().addDays(360 * 10);
        db[":f_active"] = 1;
        db.insert("b_cards_discount", false);
    }
    db[":f_code"] = "VISIT";
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (!db.nextRow()) {
        o["noconfig"] = 1;
    } else {
        o["current"] = o["visit"].toInt() % db.getInt("f_value");
    }
    o["reply"] = 1;
}

void C5WaiterServer::processPrintRemovedService(QJsonObject &o)
{
    C5PrintRemovedServiceThread *pr = new C5PrintRemovedServiceThread(fIn["id"].toString());
    pr->start();
}

void C5WaiterServer::processReturnTaxReceipt(QJsonObject &o)
{
    C5Database db(C5Config::dbParams());
    db[":f_id"] = fIn["order"].toString();
    db.exec("select * from o_tax where f_id=:f_id");
    QString crn, rseq;
    if (db.nextRow()) {
        crn = db.getString("f_devnum");
        rseq = db.getString("f_receiptnumber");
    } else {
        o["reply"] = 0;
        o["msg"] = tr("No tax receipt exists for this order");
        return;
    }
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printTaxback(rseq, crn, jsnin, jsnout, err);
    o["reply"] = result == pt_err_ok ? 1 : 0;
    o["msg"] = result == pt_err_ok ? tr("Success") : err;
    db[":f_id"] = db.uuid();
    db[":f_order"] = fIn["order"].toString();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    if (result != pt_err_ok) {
    } else {
        db[":f_fiscal"] = QVariant();
        db[":f_receiptnumber"] = QVariant();
        db[":f_time"] = QVariant();
        db.update("o_tax", "f_id", fIn["order"].toString());
    }
}

void C5WaiterServer::processTaxReport(QJsonObject &o)
{
    Q_UNUSED(o);
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(QDateTime::fromString(fIn["d1"].toString(), FORMAT_DATE_TO_STR),
                            QDateTime::fromString(fIn["d2"].toString(), FORMAT_DATE_TO_STR),
                            fIn["type"].toInt(), jsnin, jsnout, err);
    C5Database db(C5Config::dbParams());
    db[":f_id"] = db.uuid();
    db[":f_order"] = QString("Report %1").arg(fIn["type"].toInt() == report_x ? "X" : "Z");
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
}

void C5WaiterServer::writeCashDoc(C5StoreDraftWriter &dw, const QString &uuid, const QString id, QString &err,
                                  double amount, int staff, int cashboxid, QDate dateCash)
{
    QString cashdocid;
    if (!dw.writeAHeader(cashdocid, id, DOC_STATE_SAVED, DOC_TYPE_CASH,
                         staff, dateCash, QDate::currentDate(),
                         QTime::currentTime(), 0, amount,
                         id, 1, 1)) {
        err = dw.fErrorMsg;
    }
    if (!dw.writeAHeaderCash(cashdocid, cashboxid,
                             0, 1, "", uuid)) {
        err = dw.fErrorMsg;
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdocid, cashboxid, 1,
                       id,
                       amount, cashUUID, 1)) {
        err = dw.fErrorMsg;
    }
}
