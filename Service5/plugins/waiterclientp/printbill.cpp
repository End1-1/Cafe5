#include "printbill.h"
#include "database.h"
#include "printtaxn.h"
#include "c5printreceiptthread.h"
#include "c5config.h"
#include "c5translator.h"
#include "c5utils.h"
#include "logwriter.h"

PrintBill::PrintBill()
{

}

bool PrintBill::printBill2(Database *d, bool printFiscal, const QString &orderid, QString printerName, int language, QString &err)
{
    Database db(*d);
    QMap<QString, QVariant> headerInfo;
    db[":f_id"] = orderid;
    if (!db.exec("select o.*, concat_ws(' ', u.f_last, u.f_first) as f_currentstaffname,"
                 "t.f_name as f_tablename, h.f_name as f_hallname "
                 "from o_header o "
                 "left join h_tables t on t.f_id=o.f_table "
                 "left join h_halls h on h.f_id=o.f_hall "
                 "left join s_user u on u.f_id=o.f_currentstaff "
                 "where o.f_id=:f_id")) {
        err = db.lastDbError();
        return false;
    }
    if (!db.next()) {
        err = tr_am("No order with this id");
        return false;
    }
    db.rowToMap(headerInfo);

    QList<QMap<QString, QVariant> > bodyInfo;
    db[":f_header"] = orderid;
    if (!db.exec("select f_state, f_dish, f_price, f_canservice, f_candiscount, ob.f_discount, "
                 "d.f_name as f_dishname, d.f_adgt, d.f_hourlypayment, d.f_extra, "
                 "sum(f_qty1) as f_qty1, sum(f_qty2) as f_qty2, "
                 "sum(f_total) as f_total, f_adgcode, ob.f_comment "
                 "from o_body ob "
                 "left join d_dish d on d.f_id=ob.f_dish "
                 "where f_header=:f_header "
                 "group by f_dish, f_state, f_price, ob.f_discount ")) {
        err = db.lastDbError();
        return false;
    }
    while (db.next()) {
        QMap<QString, QVariant> m;
        db.rowToMap(m);
        bodyInfo.append(m);
    }

    if (printFiscal){
        bool printtax = false;
        db[":f_id"] = orderid;
        db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
        if (db.next()) {
            if (db.integer(0) == 0) {
                printtax = true;
            }
        } else {
            printtax = true;
        }
        if (printtax) {
            if (printTax(db, headerInfo, bodyInfo, err) != pt_err_ok) {
                return false;
            }
        }
    }
    //jh["f_idramphone"] = C5Config::idramPhone();

    int paperWidth = 700;
    db[":f_name"] = printerName;
    db.exec("select f_id from s_settings_names where f_name=:f_name");
    if (db.next()) {
        int s = db.integer("f_id");
        db[":f_settings"] = s;
        db[":f_key"] = param_local_receipt_printer;
        db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
        if (db.next()) {
            printerName = db.string(0);
        }
        db[":f_settings"] = s;
        db[":f_key"] = param_print_paper_width;
        db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
        if (db.next()) {
            paperWidth = db.string(0).toInt() == 0 ? 700 : db.string(0).toInt();
        }
    }

    db[":f_alias"] = printerName;
    db.exec("select * from d_print_aliases where f_alias=:f_alias");
    if (db.next()) {
        if (!db.string("f_printer").isEmpty()) {
            printerName = db.string("f_printer");
        }
    }


    LogWriter::write(LogWriterLevel::verbose, "", QString("Start printing on %1").arg(printerName));
    C5PrintReceiptThread pr(orderid, headerInfo, bodyInfo, printerName, language, paperWidth);
    pr.fBill = headerInfo["f_amountcash"].toDouble() < 0.001
            && headerInfo["f_amountcard"].toDouble() < 0.001
            && headerInfo["f_amountprepaid"].toDouble() < 0.001
            && headerInfo["f_amountpayx"].toDouble() < 0.001
            && headerInfo["f_amountidram"].toDouble() < 0.001
            && headerInfo["f_amountbank"].toDouble() < 0.001
            && headerInfo["f_amountother"].toDouble() < 0.001;
    pr.fIdram[param_idram_name] = C5Config::getValue(param_idram_name);
    pr.fIdram[param_idram_id] = C5Config::getValue(param_idram_id);
    pr.fIdram[param_idram_phone] = C5Config::getValue(param_idram_phone);
    pr.fIdram[param_idram_tips] = C5Config::getValue(param_idram_tips);
    pr.fIdram[param_idram_tips_wallet] = C5Config::getValue(param_idram_tips_wallet);
    if (!pr.print(db.params())) {
        err = pr.fError;
    }


    LogWriter::write(LogWriterLevel::verbose, "", "update precheck");
    if (err.isEmpty()) {
        db[":f_precheck"] = abs(headerInfo["f_precheck"].toInt()) + 1;
        db[":f_print"] = abs(headerInfo["f_print"].toInt()) + 1;
        db.update("o_header", "f_id", orderid);
    }
    return err.isEmpty();
}

int PrintBill::printTax(Database &db, QMap<QString, QVariant> &header, QList<QMap<QString, QVariant> > &body, QString &err)
{

    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin());
    for (int i = 0; i < body.count(); i++) {
        const QMap<QString, QVariant> &m = body.at(i);
        if (m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        pt.addGoods(C5Config::taxDept(),
                m["f_adgt"].toString(),
                m["f_dish"].toString(),
                m["f_dishname"].toString(),
                m["f_price"].toDouble(),
                m["f_qty2"].toDouble(),
                m["f_discount"].toDouble() * 100);
    }
    if (header["f_amountservice"] > 0.001) {
        pt.addGoods(C5Config::taxDept(),
                    "5901",
                    "001",
                    QString("%1 %2%").arg(tr_am("Service")).arg(float_str(header["f_servicefactor"].toDouble()*100, 2)),
                    header["f_amountservice"].toDouble(), 1.0, 0);
    }
    QString jsonIn, jsonOut;
    int result = 0;
    double cardamount = header["f_amountcard"].toDouble()
            + header["f_amountidram"].toDouble()
            + header["f_amountpayx"].toDouble();
    result = pt.makeJsonAndPrint(cardamount, 0, jsonIn, jsonOut, err);
    db[":f_id"] = db.uuid();
    db[":f_order"] = header["f_id"].toString();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log");
    if (__c5config.getValue(param_debuge_mode).toInt() == 1) {
        QSqlQuery q(db.fSqlDatabase);
        pt.saveTimeResult(header["f_id"].toString(), q);
    }
    if (result == pt_err_ok) {
        QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = header["f_id"].toString();
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = header["f_id"].toString();
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr_am("(F)");
        db[":f_time"] = time;
        db.insert("o_tax");
    } else {
        err = jsonOut;
    }
    return result;
}
