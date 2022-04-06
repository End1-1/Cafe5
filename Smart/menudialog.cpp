#include "menudialog.h"
#include "ui_menudialog.h"
#include "touchselecttaxreport.h"
#include "touchentertaxreceiptnumber.h"
#include "printtaxn.h"
#include "calendar.h"
#include "c5printing.h"
#include <QFile>

MenuDialog::MenuDialog() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::MenuDialog)
{
    ui->setupUi(this);
}

MenuDialog::~MenuDialog()
{
    delete ui;
}

void MenuDialog::on_btnExit_clicked()
{
    if (C5Message::question(tr("Confirm to close application"))) {
        qApp->quit();
    }
}

void MenuDialog::on_btnFiscalZReport_clicked()
{
    accept();
    int reporttype;
    if (TouchSelectTaxReport::getReportType(reporttype) == false) {
        return;
    }
    QDate date1, date2;
    if (!Calendar::getDate2(date1, date2)) {
        return;
    }
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(QDateTime(date1), QDateTime(date2), reporttype, jsnin, jsnout, err);
    C5Database db(C5Config::dbParams());
    db[":f_id"] = db.uuid();
    db[":f_order"] = QString("Report %1").arg(reporttype == report_x ? "X" : "Z");
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
}

void MenuDialog::on_btnReturnFiscalReceipt_clicked()
{
    accept();
    QString number;
    if (TouchEnterTaxReceiptNumber::getTaxReceiptNumber(number)) {
        C5Database db(C5Config::dbParams());
        db[":f_receiptnumber"] = number;
        db.exec("select f_id from o_tax where cast(f_receiptnumber as signed)=cast(:f_receiptnumber as signed)");
        QString uuid = "--";
        if (db.nextRow()) {
            uuid = db.getString("f_id");
        }
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsnin, jsnout, err, crn;
        int result;
        result = pt.printTaxback(number, crn, jsnin, jsnout, err);

        db[":f_id"] = db.uuid();
        db[":f_order"] = uuid;
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_in"] = jsnin;
        db[":f_out"] = jsnout;
        db[":f_err"] = err;
        db[":f_result"] = result;
        db.insert("o_tax_log", false);
        if (result != pt_err_ok) {
            QSqlQuery *q = new QSqlQuery(db.fDb);
            pt.saveTimeResult(uuid, *q);
            delete q;
            C5Message::error(err);
        } else {
            if (uuid != "--") {
                db[":f_fiscal"] = QVariant();
                db[":f_receiptnumber"] = QVariant();
                db[":f_time"] = QVariant();
                db.update("o_tax", "f_id", uuid);
            }
            C5Message::info(tr("Complete"));
        }
    }
}

void MenuDialog::on_btnReportByOrder_clicked()
{
    accept();
    QDate date1, date2;
    if (!Calendar::getDate2(date1, date2)) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_datecash1"] = date1;
    db[":f_datecash2"] = date2;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select concat(oh.f_prefix, oh.f_hallid) as f_ordernum, t.f_receiptnumber, oh.f_amounttotal, oh.f_amountcash, oh.f_amountcard, oh.f_amountidram, oh.f_amountother "
            "from o_header oh "
            "left join o_tax t on t.f_id=oh.f_id  "
            "where oh.f_state=:f_state and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "order by oh.f_dateopen, oh.f_timeopen ");

    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("End of day"));
    p.br();
    p.ctext(date1.toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(date2.toString(FORMAT_DATE_TO_STR));
    p.br();
    double total = 0, totalcash = 0, totalcard=0, totalidram=0, totalother=0;

    while (db.nextRow()) {
        p.ltext(QString("[%1]%2").arg((db.getString("f_receiptnumber").toInt() == 0 ? "*" : db.getString("f_receiptnumber")), db.getString("f_ordernum")), 0);
        p.rtext(QString("%1 %2").arg(db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR), db.getTime("f_dateclose").toString(FORMAT_TIME_TO_SHORT_STR)));
        p.br();
        total += db.getDouble("f_amounttotal");
        if (db.getDouble("f_amountcash") > 0.001) {
            totalcash += db.getDouble("f_amountcash");
            p.ltext(tr("Cash"), 0);
            p.rtext(float_str(db.getDouble("f_amountcash"), 2));
            p.br();
        }
        if (db.getDouble("f_amountcard") > 0.001) {
            totalcard += db.getDouble("f_amountcard");
            p.ltext(tr("Card"), 0);
            p.rtext(float_str(db.getDouble("f_amountcard"), 2));
            p.br();
        }
        if (db.getDouble("f_amountidram") > 0.001) {
            totalidram += db.getDouble("f_amountidram");
            p.ltext(tr("Idram"), 0);
            p.rtext(float_str(db.getDouble("f_amountidram"), 2));
            p.br();
        }
        if (db.getDouble("f_amountother") > 0.001) {
            totalother += db.getDouble("f_amountother");
            p.ltext(tr("Other"), 0);
            p.rtext(float_str(db.getDouble("f_amountother"), 2));
            p.br();
        }
        p.line();
        p.br();
    }

    p.line(2);
    p.br();
    p.line(2);
    p.br();

    p.setFontSize(24);
    p.setFontBold(true);
    p.ltext(tr("Total"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    if (totalcash > 0.001) {
        p.ltext(tr("Cash"), 0);
        p.rtext(float_str(totalcash, 2));
        p.br();
    }
    if (totalcard > 0.001) {
        p.ltext(tr("Card"), 0);
        p.rtext(float_str(totalcard, 2));
        p.br();
    }
    if (totalidram > 0.001) {
        p.ltext(tr("Idram"), 0);
        p.rtext(float_str(totalidram, 2));
        p.br();
    }
    if (totalother > 0.001) {
        p.ltext(tr("Other"), 0);
        p.rtext(float_str(totalother, 2));
        p.br();
    }

    p.setFontSize(18);
    p.setFontBold(false);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}

void MenuDialog::on_btnBack_clicked()
{
    accept();
}

void MenuDialog::on_btnPrintReport_clicked()
{
    accept();
    QDate d1, d2;
    if (!Calendar::getDate2(d1, d2)) {
        return;
    }
    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("End of day"));
    p.br();
    p.ctext(d1.toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(d2.toString(FORMAT_DATE_TO_STR));
    p.br();
    double total = 0;

    C5Database dd(fDBParams);
    dd[":f_datecash1"] = d1;
    dd[":f_datecash2"] = d2;
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd.exec("select count(f_id) from o_header h "
            "where h.f_state=:f_stateh  and h.f_datecash between :f_datecash1 and :f_datecash2 ");
    dd.nextRow();
    int totalQty = dd.getInt(0);
    dd[":f_datecash1"] = d1;
    dd[":f_datecash2"] = d2;
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd[":f_stated"] = DISH_STATE_OK;
    dd.exec("select d.f_name, sum(b.f_qty1) as f_qty, b.f_price, sum(b.f_qty1*b.f_price) as f_total "
            "from o_body b "
            "inner join o_header h on h.f_id=b.f_header "
            "left join d_dish d on d.f_id=b.f_dish "
            "where h.f_state=:f_stateh and b.f_state=:f_stated and h.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1, 3 ");
    p.setFontBold(false);
    p.setFontSize(22);
    while (dd.nextRow()) {
        if (p.checkBr(p.fLineHeight + 2)) {
            p.br();
        }
        total += dd.getDouble("f_total");
        p.ltext(dd.getString("f_name"), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3").arg(float_str(dd.getDouble("f_qty"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_total"), 2)), 0);
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.setFontSize(28);
    p.br();
    p.br();
    p.ltext(tr("Quantity of orders"), 0);
    p.rtext(QString::number(totalQty));
    p.br();
    p.br();
    p.ltext(tr("Total today"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}
