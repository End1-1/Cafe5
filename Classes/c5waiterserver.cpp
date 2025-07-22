#include "c5waiterserver.h"
#include "logwriter.h"
#include "c5printjson.h"
#include "jsons.h"
#include <QTcpServer>
#include <QtHttpServer>
#include <QDebug>
#include <QJsonArray>
#include <QDir>
#include <QLibrary>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>
#include <QJsonObject>

C5WaiterServer::C5WaiterServer()
{
    mServer = new QHttpServer();
    mServer->route("/printreceipt", [](const QHttpServerRequest &request) {
        return QHttpServerResponse(QJsonObject{{"status", 1}});
    });
    mServer->route("/printservice", [](const QHttpServerRequest &request) {
        QJsonObject jo = __bytejson(request.body());
        auto pj = new C5PrintJson(jo["data"].toArray());
        pj->start();
        return QHttpServerResponse(QJsonObject{{"status", 1}});
    });
    mServer->route("/mobile", [](const QHttpServerRequest &request) {
        //TODO: CHECK L2
        /*
         *     C5Database db;
        db.exec("select * from sys_mobile where f_state=1");
        QList<int> ids;
        while (db.nextRow()) {
         ids.append(db.getInt("f_id"));
         QJsonObject jo = __strjson(db.getString("f_order"));
         if (jo["action"].toString() == "printservice") {
             C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintService(QJsonObject)));
             sh->bind("cmd", sm_printservice);
             sh->bind("order", jo["id"].toString());
             sh->bind("booking", 0);
             sh->bind("alias", __c5config.getValue(param_force_use_print_alias).toInt());
             sh->send();
             //logRecord(fUser->fullName(), jo["id"].toString(), "", "Send to cooking", "", "");
         }
        }
        for (int id : ids) {
         db[":f_id"] = id;
         db.exec("update sys_mobile set f_state=2 where f_id=:f_id");
        }*/
        return QHttpServerResponse(QJsonObject{{"status", 1}});
    });
    mServer->route("/taxreport", [](const QHttpServerRequest &request) {
        /*
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                     C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsnin, jsnout, err;
        int result;
        result = pt.printReport(QDate::fromString(fIn["d1"].toString(), FORMAT_DATE_TO_STR_MYSQL),
                                QDate::fromString(fIn["d2"].toString(), FORMAT_DATE_TO_STR_MYSQL),
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
        */
        return QHttpServerResponse(QJsonObject{{"status", 1}});
    });
    mServer->route("/taxreturn", [](const QHttpServerRequest &request) {
        /*
        C5Database db(C5Config::dbParams());
        db[":f_id"] = fIn["order"].toString();
        db.exec("select * from o_tax_log where f_order=:f_id and f_state=1");
        QJsonObject jo;
        if (db.nextRow()) {
            jo = __strjson(db.getString("f_out"));
        } else {
            o["reply"] = 0;
            o["msg"] = tr("No tax receipt exists for this order");
            return;
        }
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                     C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsnin, jsnout, err;
        int result;
        result = pt.printTaxback(jo["rseq"].toInt(), jo["crn"].toString(), jsnin, jsnout, err);
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
        } */
        return QHttpServerResponse(QJsonObject{{"status", 1}});
    });
    mTcpServer = new QTcpServer();
    if (!mTcpServer->listen(QHostAddress::Any, 8080)) {
        LogWriter::write(LogWriterLevel::errors, "C5WAITERSERVER", "could not listen 8080 port.");
    }
    if (!mServer->bind(mTcpServer)) {
        LogWriter::write(LogWriterLevel::errors, "C5WAITERSERVER", "could bind tcp server to http server.");
    }
}

C5WaiterServer::~C5WaiterServer()
{
    mServer->deleteLater();
    mTcpServer->deleteLater();
}

// void C5WaiterServer::openActiveOrder(QJsonObject &jh, QJsonArray &jb, QJsonArray &jt, C5ServerHandler &srh)
// {
//     QMap<QString, QVariant> bv;
//     srh.fDb[":f_lock"] = 1;
//     srh.fDb[":f_lockSrc"] = fIn["host"].toString();
//     srh.fDb[":f_id"] = fIn["table"].toInt();
//     srh.fDb.exec("update h_tables set f_lock=:f_lock, f_lockSrc=:f_lockSrc where f_id=:f_id");
//     QJsonArray jo;
//     if (fIn["orderid"].toString().isEmpty()) {
//         bv[":f_table"] = fIn["table"].toInt();
//         bv[":f_state"] = ORDER_STATE_OPEN;
//         srh.getJsonFromQuery("select o.f_id from o_header o  where o.f_table=:f_table and o.f_state=:f_state limit 1 ", jo, bv);
//         if (jo.count() == 0) {
//             srh.fDb[":f_id"] = fIn["table"].toInt();
//             srh.fDb.exec("select f_name from h_tables where f_id=:f_id");
//             if (srh.fDb.nextRow()) {
//                 jh["f_tablename"] = srh.fDb.getString(0);
//             }
//             srh.fDb[":f_id"] = jt.at(0).toObject()["f_hall"].toString();
//             srh.fDb.exec("select f_name from h_halls where f_id=:f_id");
//             if (srh.fDb.nextRow()) {
//                 jh["f_hallname"] = srh.fDb.getString(0);
//             }
//             jh["f_id"] = "";
//             jh["f_hall"] = jt.at(0).toObject()["f_hall"];
//             jh["f_table"] = QString::number(fIn["table"].toInt());
//             jh["f_state"] = QString::number(ORDER_STATE_OPEN);
//             jh["f_dateopen"] = current_date;
//             jh["f_timeopen"] = current_time;
//         } else {
//             jh["f_id"] = jo.at(0).toObject()["f_id"].toString();
//             C5WaiterOrderDoc w(jh["f_id"].toString(), srh.fDb);
//             jh = w.fHeader;
//             jb = w.fItems;
//         }
//     } else {
//         jh["f_id"] = jo.at(0).toObject()["f_id"].toString();
//         C5WaiterOrderDoc w(jh["f_id"].toString(), srh.fDb);
//         jh = w.fHeader;
//         jb = w.fItems;
//     }
// }

// void C5WaiterServer::processCloseOrder(QJsonObject &o, C5Database &db)
// {
//     QString err;
//     QJsonArray jb = fIn["body"].toArray();
//     QJsonObject jh = fIn["header"].toObject();
//     int orderstate = jh["f_state"].toString().toInt();
//     if (orderstate == ORDER_STATE_OPEN) {
//         orderstate = ORDER_STATE_CLOSE;
//     }
//     // CHECKING ALL ITEMS THAT WAS PRINTED
//     for (int i = 0; i < jb.count(); i++) {
//         QJsonObject jo = jb[i].toObject();
//         if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
//             continue;
//         }
//         if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
//             err += tr("All service must be complited");
//             break;
//         }
//     }
//     if (jh["f_print"].toString().toInt() < 1) {
//         err += tr("Receipt was not printed");
//     }
//     // Get settgins
//     QMap<int, QString> settings;
//     db[":f_id"] = jh["f_hall"].toString().toInt();
//     db.exec("select f_settings from h_halls where f_id=:f_id");
//     if (!db.nextRow()) {
//         o["reply"] = 0;
//         o["msg"] = QString("%1. %2: %3").arg(tr("Get settings failed")).arg("Settings id").arg(jh["f_hall"].toString());
//         return;
//     }
//     int settings_id = db.getInt(0);
//     db[":f_id"] = jh["f_table"].toString().toInt();
//     db.exec("select f_special_config from h_tables where f_id=:f_id");
//     db.nextRow();
//     if (db.getInt(0) > 0) {
//         settings_id = db.getInt(0);
//     }
//     db[":f_settings"] = settings_id;
//     db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
//     while (db.nextRow()) {
//         settings[db.getInt(0)] = db.getString(1);
//     }
//     QDate dateCash = QDate::currentDate();
//     int dateShift = settings[param_date_cash_shift].toInt();
//     bool isAuto = settings[param_date_cash_auto].toInt() > 0;
//     if (isAuto) {
//         QTime t = QTime::fromString(settings[param_working_date_change_time], "HH:mm:ss");
//         if (t.isValid()) {
//             if (QTime::currentTime() < t) {
//                 dateCash = dateCash.addDays(-1);
//             }
//         }
//     } else {
//         dateCash = QDate::fromString(settings[param_date_cash], FORMAT_DATE_TO_STR_MYSQL);
//         if (!dateCash.isValid()) {
//             o["reply"] = 0;
//             o["msg"] = tr("Date cash set manual and it is not valid.");
//             return;
//         }
//     }
//     if (err.isEmpty()) {
//         jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
//         jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR);
//         jh["f_datecash"] = dateCash.toString(FORMAT_DATE_TO_STR);
//         db[":f_state"] = orderstate;
//         db[":f_dateclose"] = QDate::currentDate();
//         db[":f_datecash"] = QDate::fromString(jh["f_datecash"].toString(), FORMAT_DATE_TO_STR);
//         db[":f_timeclose"] = QTime::fromString(jh["f_timeclose"].toString(), FORMAT_TIME_TO_STR);
//         db[":f_staff"] = jh["f_currentstaff"].toString().toInt();
//         db.update("o_header", where_id(jh["f_id"].toString()));
//         db[":f_lock"] = 0;
//         db[":f_lockSrc"] = "";
//         db.update("h_tables", where_id(jh["f_table"].toString().toInt()));
//         if (dateShift == 0) {
//             db[":f_time"] = QTime::fromString(jh["f_timeclose"].toString(), FORMAT_TIME_TO_STR);
//             db.exec("select f_shift from s_salary_shift_time where :f_time between f_start and f_end");
//             if (db.nextRow()) {
//                 dateShift = db.getInt("f_shift");
//             } else {
//                 dateShift = 1;
//             }
//         }
//         db[":f_id"] = jh["f_id"].toString();
//         db[":f_shift"] = dateShift;
//         db.exec("update o_header set f_shift=:f_shift where f_id=:f_id");
//         if (jh["f_otherid"].toString().toInt() == PAYOTHER_DEBT) {
//             db[":f_order"] = jh["f_id"].toString();
//             db.exec("select f_costumer, f_govnumber from o_header_options co "
//                     "inner join b_car c on c.f_id=co.f_car "
//                     "where f_order=:f_order");
//             if (db.nextRow()) {
//                 db[":f_costumer"] = db.getInt(0);
//                 db[":f_order"] = jh["f_id"].toString();
//                 db[":f_amount"] = jh["f_amountother"].toString().toDouble() * -1;
//                 db[":f_date"] = dateCash;
//                 db[":f_govnumber"] = db.getString(1);
//                 db.insert("b_clients_debts", false);
//             }
//         }
//         if (orderstate == ORDER_STATE_CLOSE) {
//             if (jh["f_bonusid"].toString().toInt() > 0) {
//                 db[":f_id"] = jh["f_id"].toString();
//                 qDebug() << jh;
//                 switch (jh["f_bonustype"].toString().toInt()) {
//                     case CARD_TYPE_DISCOUNT:
//                         db[":f_data"] = jh["f_amountdiscount"].toString().toDouble();
//                         break;
//                     case CARD_TYPE_ACCUMULATIVE:
//                         db[":f_data"] = jh["f_amounttotal"].toString().toDouble() * (jh["f_bonusvalue"].toString().toDouble() / 100);
//                         break;
//                 }
//                 db.exec("update b_history set f_data=:f_data where f_id=:f_id");
//             }
//             C5StoreDraftWriter dw(db);
//             dw.transferToHotel(db, jh["f_id"].toString(), err);
//             if (settings[param_waiter_automatially_storeout].toInt() > 0) {
//                 dw.writeStoreOfSale(jh["f_id"].toString(), err, DOC_STATE_SAVED);
//             }
//             auto *cbc = Configs::construct<CashboxConfig>(db.dbParams(), 2);
//             if (settings[param_autoinput_salecash].toInt() == 1) {
//                 QString headerPrefix;
//                 int headerId;
//                 if (!dw.hallId(headerPrefix, headerId, jh["f_hall"].toString().toInt())) {
//                     err = dw.fErrorMsg;
//                 }
//                 if (jh["f_amountcash"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountcash"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash1, dateCash);
//                 }
//                 if (jh["f_amountcard"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountcard"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash2, dateCash);
//                 }
//                 if (jh["f_amountbank"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountbank"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash2, dateCash);
//                 }
//                 if (jh["f_amountprepaid"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountprepaid"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash2, dateCash);
//                 }
//                 if (jh["f_amountidram"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountidram"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash2, dateCash);
//                 }
//                 if (jh["f_amountpayx"].toString().toDouble() > 0.0001) {
//                     writeCashDoc(dw, jh["f_id"].toString(), QString("%1%2").arg(headerPrefix, headerId), err,
//                                  jh["f_amountpayx"].toString().toDouble(), jh["f_currentstaff"].toString().toInt(),
//                                  cbc->cash2, dateCash);
//                 }
//             }
//         } else {
//             C5WaiterOrderDoc::removeDocument(db, jh["f_id"].toString());
//         }
//     }
//     o["reply"] = err.isEmpty() ? 1 : 0;
//     o["msg"] = err;
// }

// int C5WaiterServer::printTax(const QMap<QString, QVariant> &header, const QList<QMap<QString, QVariant> > &body,
//                              const QStringList emarks, QString &err)
// {
//     QElapsedTimer et;
//     et.start();
//     C5Database db;
//     PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
//                  C5Config::taxCashier(), C5Config::taxPin(),  this);
//     pt.fEmarks.append(emarks);
//     for (int i = 0; i < body.count(); i++) {
//         const QMap<QString, QVariant> &m = body.at(i);
//         if (m["f_state"].toInt() != DISH_STATE_OK) {
//             continue;
//         }
//         pt.addGoods(C5Config::taxDept().toInt(),
//                     dbdishpart2->adgcode(dbdish->part2(m["f_dish"].toInt())),
//                     m["f_dish"].toString(),
//                     dbdish->name(m["f_dish"].toInt()),
//                     m["f_price"].toDouble(),
//                     m["f_qty2"].toDouble(),
//                     m["f_discount"].toDouble() * 100);
//     }
//     if (header["f_amountservice"].toDouble() > 0.001) {
//         pt.addGoods(C5Config::taxDept().toInt(),
//                     "5901",
//                     "001",
//                     QString("%1 %2%").arg(tr("Service")).arg(float_str(header["f_servicefactor"].toDouble() * 100, 2)),
//                     header["f_amountservice"].toDouble(), 1.0, 0);
//     }
//     QString jsonIn, jsonOut;
//     int result = 0;
//     double cardamount = header["f_amountcard"].toDouble()
//                         + header["f_amountidram"].toDouble()
//                         + header["f_amountpayx"].toDouble();
//     result = pt.makeJsonAndPrint(cardamount, header["f_amountprepaid"].toDouble(), jsonIn, jsonOut, err);
// #ifdef QT_DEBUG
//     //#define return_5
//     if (result != 0) {
// #ifdef return_5
//         result = -5;
//         jsonOut = "{\"address\": \"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Հյուսիսային պողոտա 1/1 \", "
//                   "\"change\": 0, \"crn\": \"53241359\", \"fiscal\": \"81326142\", "
//                   "\"lottery\": \"\", \"prize\": 0, \"rseq\": 7174, \"sn\": \"00022164315\", "
//                   "\"taxpayer\": \"«ՍՄԱՐՏ ՊԱՐԿԻՆԳ»\", \"time\": 1733052744891, \"tin\": \"02853428\", \"total\": 1}";
// #else
//         result = 0;
//         jsonOut = "{\"address\": \"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Հյուսիսային պողոտա 1/1 \", "
//                   "\"change\": 0, \"crn\": \"53241359\", \"fiscal\": \"81326142\", "
//                   "\"lottery\": \"\", \"prize\": 0, \"rseq\": 7174, \"sn\": \"00022164315\", "
//                   "\"taxpayer\": \"«ՍՄԱՐՏ ՊԱՐԿԻՆԳ»\", \"time\": 1733052744891, \"tin\": \"02853428\", \"total\": 1}";
//     }
// #endif
// #endif
//         QJsonObject jtax;
//         QJsonParseError jsonErr;
//         jtax["f_order"] = header["f_id"].toString();
//         jtax["f_elapsed"] = et.elapsed();
//         jtax["f_in"] = QJsonDocument::fromJson(jsonIn.toUtf8(), &jsonErr).object();
//         jtax["f_out"] = QJsonDocument::fromJson(jsonOut.toUtf8()).object();;
//         jtax["f_err"] = err;
//         jtax["f_result"] = result;
//         jtax["f_state"] = result == pt_err_ok ? 1 : 0;
//         if (jsonErr.error != QJsonParseError::NoError) {
//         }
//         QString jtaxstr = QString(QJsonDocument(jtax).toJson(
//                                       QJsonDocument::Compact));
//         jtaxstr.replace("\\\"", "\\\\\"");
//         jtaxstr.replace("'", "\\'");
//         if (!db.exec(QString("call sf_create_shop_tax('%1')").arg(jtaxstr))) {
//         }
//         return result;
//     }

//     bool C5WaiterServer::printReceipt(QString &err, C5Database &db, bool isBill, bool alias)
//     {
//         if (fIn["close"].toInt() > 0) {
//             db[":f_id"] = fIn["order"].toString();
//             db.exec("select f_otherid from o_header where f_id=:f_id");
//             if (db.nextRow()) {
//                 if (db.getInt("f_otherid") == PAYOTHER_PRIMECOST) {
//                     C5WaiterOrderDoc w(fIn["order"].toString(), db);
//                     w.calculateSelfCost(db);
//                 }
//             }
//         }
//         QMap<QString, QVariant> headerInfo;
//         db[":f_id"] = fIn["order"].toString();
//         if (!db.exec("select o.*, concat_ws(' ', u.f_last, u.f_first) as f_currentstaffname,"
//                      "t.f_name as f_tablename, h.f_name as f_hallname "
//                      "from o_header o "
//                      "left join h_tables t on t.f_id=o.f_table "
//                      "left join h_halls h on h.f_id=o.f_hall "
//                      "left join s_user u on u.f_id=o.f_currentstaff "
//                      "where o.f_id=:f_id")) {
//             err = db.fLastError;
//             return false;
//         }
//         if (!db.nextRow()) {
//             err = tr("No order with this id");
//             return false;
//         }
//         db.rowToMap(headerInfo);
//         QList<QMap<QString, QVariant> > bodyInfo;
//         db[":f_header"] = fIn["order"].toString();
//         if (!db.exec("select f_state, f_dish, f_price, f_canservice, f_candiscount, ob.f_discount, "
//                      "d.f_name as f_dishname, d.f_adgt, d.f_hourlypayment, d.f_extra, "
//                      "sum(f_qty1) as f_qty1, sum(f_qty2) as f_qty2, "
//                      "sum(f_total) as f_total, f_adgcode, ob.f_comment, ob.f_emarks "
//                      "from o_body ob "
//                      "left join d_dish d on d.f_id=ob.f_dish "
//                      "where f_header=:f_header "
//                      "group by f_dish, f_state, f_price, ob.f_discount "
//                      "order by ob.f_row ")) {
//             err = db.fLastError;
//             return false;
//         }
//         while (db.nextRow()) {
//             QMap<QString, QVariant> m;
//             db.rowToMap(m);
//             bodyInfo.append(m);
//         }
//         QStringList emarks;
//         db[":f_header"] = fIn["order"].toString();
//         if (!db.exec("select f_emarks from o_body where f_state=1 and f_header=:f_header")) {
//             err = db.fLastError;
//             return false;
//         }
//         while (db.nextRow()) {
//             if (db.getString("f_emarks").length() > 0) {
//                 emarks.append(db.getString("f_emarks"));
//             }
//         }
//         if (fIn["printtax"].toInt() == 1 ) {
//             bool printtax = false;
//             db[":f_id"] = fIn["order"].toString();
//             db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
//             if (db.nextRow()) {
//                 if (db.getInt(0) == 0) {
//                     printtax = true;
//                 }
//             } else {
//                 printtax = true;
//             }
//             if (printtax) {
//                 QStringList emarks;
//                 db[":f_header"] = fIn["order"].toString();
//                 db.exec("select f_emarks from o_body where f_state=1 and f_header=:f_header");
//                 while (db.nextRow()) {
//                     if (db.getString("f_emarks").length() > 0) {
//                         emarks.append(db.getString("f_emarks"));
//                     }
//                 }
//                 if (printTax(headerInfo, bodyInfo, emarks, err) != pt_err_ok) {
//                     return false;
//                 }
//             }
//         }
// #ifdef QT_DEBUG
//         qDebug() << "Error after printtax" << err;
//         err.clear();
// #endif
//         // CHECKING FOR RECEIPT PRINT COUNT
//         //    if (jh["f_print"].toString().toInt() > 0) {
//         //        if (!checkPermission(jh["f_currentstaff"].toString().toInt(), cp_t5_multiple_receipt, srh.fDb)) {
//         //            err = tr("You cannot print more then 1 copies of receipt");
//         //            return false;
//         //        }
//         //    }
//         // CHECKING ALL ITEMS THAT WAS PRINTED
//         //    for (int i = 0; i < jb.count(); i++) {
//         //        QJsonObject jo = jb[i].toObject();
//         //        if (jo["f_state"].toString().toInt() != DISH_STATE_OK) {
//         //            continue;
//         //        }
//         //        if (jo["f_qty1"].toString() != jo["f_qty2"].toString()) {
//         //            err += tr("All service must be complited");
//         //            break;
//         //        }
//         //    }
//         //CHECK SELFCOST
//         // CHECKING FOR TAX CASH/CARD
//         // PRINT RECEIPT
//         // TODO: CHECK FOR DESTINATION PRINTER AND REDIRECT QUERY//        jh["f_idramid"] = C5Config::idramID();
//         //        jh["f_idramphone"] = C5Config::idramPhone();
//         QString printerName = "local";
//         int paperWidth = 500;
//         db[":f_name"] = fIn["receipt_printer"].toString();
//         db.exec("select f_id from s_settings_names where f_name=:f_name");
//         if (db.nextRow()) {
//             int s = db.getInt("f_id");
//             db[":f_settings"] = s;
//             db[":f_key"] = param_local_receipt_printer;
//             db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
//             if (db.nextRow()) {
//                 printerName = db.getString(0);
//             }
//             db[":f_settings"] = s;
//             db[":f_key"] = param_print_paper_width;
//             db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
//             if (db.nextRow()) {
//                 paperWidth = db.getString(0).toInt() == 0 ? 650 : db.getString(0).toInt();
//             }
//         }
//         if (alias) {
//             db[":f_alias"] = printerName;
//             db.exec("select * from d_print_aliases where f_alias=:f_alias");
//             if (db.nextRow()) {
//                 printerName = db.getString("f_printer");
//             }
//         }
//         if (fIn["withoutprint"].toInt() == 0 && fIn["nofinalreceipt"].toInt() == 0) {
//             C5PrintReceiptThread pr(fIn["order"].toString(), headerInfo, bodyInfo,
//                                     printerName, fIn["language"].toInt(), paperWidth);
//             pr.fBill = isBill;
//             pr.fIdram[param_idram_name] = C5Config::getValue(param_idram_name);
//             pr.fIdram[param_idram_id] = C5Config::getValue(param_idram_id);
//             pr.fIdram[param_idram_phone] = C5Config::getValue(param_idram_phone);
//             pr.fIdram[param_idram_tips] = C5Config::getValue(param_idram_tips);
//             pr.fIdram[param_idram_tips_wallet] = C5Config::getValue(param_idram_tips_wallet);
//             if (!pr.print(__c5config.dbParams())) {
//                 err = pr.fError;
//             }
//         }
//         if (err.isEmpty()) {
//             if (isBill) {
//                 db[":f_id"] = fIn["order"].toString();
//                 db[":f_precheck"] = abs(headerInfo["f_precheck"].toInt()) + 1;
//                 db.exec("update o_header set f_precheck=:f_precheck where f_id=:f_id");
//             } else {
//                 db[":f_id"] = fIn["order"].toString();
//                 db[":f_print"] = abs(headerInfo["f_print"].toInt()) + 1;
//                 db.exec("update o_header set f_print=:f_print where f_id=:f_id");
//             }
//         }
//         return err.isEmpty();
//     }

//     void C5WaiterServer::writeCashDoc(C5StoreDraftWriter &dw, const QString &uuid, const QString id, QString &err,
//                                       double amount, int staff, int cashboxid, QDate dateCash)
//     {
//         QString cashdocid;
//         if (!dw.writeAHeader(cashdocid, id, DOC_STATE_SAVED, DOC_TYPE_CASH,
//                              staff, dateCash, QDate::currentDate(),
//                              QTime::currentTime(), 0, amount,
//                              id, 1, 1)) {
//             err = dw.fErrorMsg;
//         }
//         if (!dw.writeAHeaderCash(cashdocid, cashboxid,
//                                  0, 1, "", uuid)) {
//             err = dw.fErrorMsg;
//         }
//         QString cashUUID;
//         if (!dw.writeECash(cashUUID, cashdocid, cashboxid, 1,
//                            id,
//                            amount, cashUUID, 1)) {
//             err = dw.fErrorMsg;
//         }
//     }
