#include "menudialog.h"
#include "ui_menudialog.h"
#include "touchselecttaxreport.h"
#include "touchentertaxreceiptnumber.h"
#include "printtaxn.h"
#include "calendar.h"
#include "dlgcashinout.h"
#include "c5printing.h"
#include "c5user.h"
#include "dlgcashop.h"
#include "dlgsmartreports.h"
#include "sessionshistory.h"
#include "c5config.h"
#include "c5database.h"
#include "c5message.h"
#include "c5utils.h"
#include "workspace.h"
#include <QFile>
#include <QScreen>

MenuDialog::MenuDialog(Workspace *w, C5User *u) :
    C5Dialog(u),
    ui(new Ui::MenuDialog),
    fWorkspace(w)
{
    ui->setupUi(this);
    fUser = u;

    if(qApp->screens().count() < 2) {
        ui->btnCustomerDisplay->setEnabled(false);
    }
}

MenuDialog::~MenuDialog()
{
    delete ui;
}

void MenuDialog::on_btnExit_clicked()
{
    if(C5Message::question(tr("Confirm to close application"))) {
        accept();
        mUser->setProperty("session_close", true);
    }
}

void MenuDialog::on_btnFiscalZReport_clicked()
{
    accept();
    int reporttype;

    if(TouchSelectTaxReport::getReportType(reporttype, mUser) == false) {
        return;
    }

    QDate date1, date2;

    if(!Calendar::getDate2(date1, date2, mUser)) {
        return;
    }

    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(date1, date2, reporttype, jsnin, jsnout, err);
    C5Database db;
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

    if(TouchEnterTaxReceiptNumber::getTaxReceiptNumber(number, mUser)) {
        QElapsedTimer et;
        et.start();
        C5Database db;
        db[":f_receiptnumber"] = number;
        db.exec("select * from o_tax where cast(f_receiptnumber as signed)=cast(:f_receiptnumber as signed)");
        QString uuid = "--";
        QString jsnin, jsnout, err, crn;

        if(db.nextRow()) {
            uuid = db.getString("f_id");
            crn = db.getString("f_devnum");
        } else {
            C5Message::error(tr("Fiscal number not found"));
            return;
        }

        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                     C5Config::taxCashier(), C5Config::taxPin(), this);
        int result;
        result = pt.printTaxback(number.toInt(), crn, jsnin, jsnout, err);
        //        db[":f_id"] = db.uuid();
        //        db[":f_order"] = uuid;
        //        db[":f_date"] = QDate::currentDate();
        //        db[":f_time"] = QTime::currentTime();
        //        db[":f_in"] = jsnin;
        //        db[":f_out"] = jsnout;
        //        db[":f_err"] = err;
        //        db[":f_result"] = result;
        //        db.insert("o_tax_log", false);
        QJsonParseError jerr;
        QJsonDocument jod = QJsonDocument::fromJson(jsnout.toUtf8(), &jerr);

        if(jerr.error != QJsonParseError::NoError) {
            err = jerr.errorString();
            jod = QJsonDocument::fromJson(QString("{\"data\":\"" + jsnout + "\"").toUtf8(), &jerr);
        }

        QJsonObject jtax;
        jtax["f_order"] = uuid;
        jtax["f_elapsed"] = et.elapsed();
        jtax["f_in"] = QJsonDocument::fromJson(jsnin.toUtf8()).object();
        jtax["f_out"] = jod.object();
        jtax["f_err"] = err;
        jtax["f_result"] = result;
        jtax["f_state"] = result == pt_err_ok ? 1 : 0;
        QString sql = QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact));
        sql.replace("\\\"", "\\\\\"");
        sql.replace("'", "\\'");
        db.exec(QString("call sf_create_shop_tax('%1')").arg(sql));

        if(result != pt_err_ok) {
            C5Message::error(err);
        }
    }
}

void MenuDialog::on_btnReportByOrder_clicked()
{
    accept();
    QDate date1, date2;

    if(!Calendar::getDate2(date1, date2, mUser)) {
        return;
    }

    C5Database db;
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
    QPrinterInfo pi = QPrinterInfo::printerInfo(C5Config::localReceiptPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    const int bs = 14;
    font.setPointSize(bs);
    p.setFont(font);

    if(QFile::exists("./logo_receipt.png")) {
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
    double total = 0, totalcash = 0, totalcard = 0, totalidram = 0, totalother = 0;

    while(db.nextRow()) {
        p.ltext(QString("[%1]%2").arg((db.getString("f_receiptnumber").toInt() == 0 ? "*" : db.getString("f_receiptnumber")),
                                      db.getString("f_ordernum")), 0);
        p.rtext(QString("%1 %2").arg(db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR),
                                     db.getTime("f_dateclose").toString(FORMAT_TIME_TO_SHORT_STR)));
        p.br();
        total += db.getDouble("f_amounttotal");

        if(db.getDouble("f_amountcash") > 0.001) {
            totalcash += db.getDouble("f_amountcash");
            p.ltext(tr("Cash"), 0);
            p.rtext(float_str(db.getDouble("f_amountcash"), 2));
            p.br();
        }

        if(db.getDouble("f_amountcard") > 0.001) {
            totalcard += db.getDouble("f_amountcard");
            p.ltext(tr("Card"), 0);
            p.rtext(float_str(db.getDouble("f_amountcard"), 2));
            p.br();
        }

        if(db.getDouble("f_amountidram") > 0.001) {
            totalidram += db.getDouble("f_amountidram");
            p.ltext(tr("Idram"), 0);
            p.rtext(float_str(db.getDouble("f_amountidram"), 2));
            p.br();
        }

        if(db.getDouble("f_amountother") > 0.001) {
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
    p.setFontBold(true);
    p.ltext(tr("Total"), 0);
    p.rtext(float_str(total, 2));
    p.br();

    if(totalcash > 0.001) {
        p.ltext(tr("Cash"), 0);
        p.rtext(float_str(totalcash, 2));
        p.br();
    }

    if(totalcard > 0.001) {
        p.ltext(tr("Card"), 0);
        p.rtext(float_str(totalcard, 2));
        p.br();
    }

    if(totalidram > 0.001) {
        p.ltext(tr("Idram"), 0);
        p.rtext(float_str(totalidram, 2));
        p.br();
    }

    if(totalother > 0.001) {
        p.ltext(tr("Other"), 0);
        p.rtext(float_str(totalother, 2));
        p.br();
    }

    p.setFontSize(bs - 4);
    p.setFontBold(false);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(printer);
}

void MenuDialog::on_btnBack_clicked()
{
    accept();
}

void MenuDialog::on_btnPrintReport_clicked()
{
    accept();
    DlgSmartReports(mUser).exec();
}

void MenuDialog::on_btnCashin_clicked()
{
    accept();
    DlgCashOp d(true, fUser);
    d.exec();
}

void MenuDialog::on_btnCashout_clicked()
{
    accept();
    DlgCashOp d(false, fUser);
    d.exec();
}

void MenuDialog::on_btnCloseSession_clicked()
{
    accept();

    if(DlgCashinOut::cashinout(fUser) == QDialog::Accepted) {
        fUser->setProperty("session_close", true);
    }
}

void MenuDialog::on_btnSessoinMoney_clicked()
{
    accept();
    QDate d1, d2;

    if(!Calendar::getDate2(d1, d2, mUser)) {
        return;
    }

    SessionsHistory(d1, d2, mUser).exec();
}

void MenuDialog::on_btnCustomerDisplay_clicked()
{
    accept();
    fWorkspace->showCustomerDisplay();
}
