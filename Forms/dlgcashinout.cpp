#include "dlgcashinout.h"
#include "ui_dlgcashinout.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5storedraftwriter.h"
#include <QFile>

DlgCashinOut::DlgCashinOut(C5User *u) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgCashinOut)
{
    ui->setupUi(this);
    fUser = u;
    ui->tbl->setColumnWidth(0, 300);
    ui->tbl->setColumnWidth(1, 100);
    C5Database db(__c5config.dbParams());
    double balance = 0;
    db[":f_session"] = __c5config.getRegValue("session");
    //db[":f_cash"] = __c5config.cashId();
    db.exec("select sum(f_amount) as f_amount "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(hc.f_oheader)>0 and f_session=:f_session ");
    db.nextRow();
    balance += db.getDouble("f_amount");
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(tr("Input from sale")));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));

    db[":f_session"] = __c5config.getRegValue("session");
    //db[":f_cash"] = __c5config.cashId();
    db.exec("select f_amount, f_remarks "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(hc.f_oheader)=0 and f_session=:f_session and f_sign=1");
    while (db.nextRow()) {
        r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.getString("f_remarks")));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));
        balance += db.getDouble("f_amount");
    }

    db[":f_session"] = __c5config.getRegValue("session");
    //db[":f_cash"] = __c5config.cashId();
    db.exec("select f_sign*f_amount as f_amount, f_remarks "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(hc.f_oheader)=0 and f_session=:f_session and f_sign=-1");
    while (db.nextRow()) {
        r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.getString("f_remarks")));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));
        balance += db.getDouble("f_amount");
    }
    ui->lbBalance->setText(float_str(balance, 2));
    fBalance = balance;
}

int DlgCashinOut::cashinout(C5User *u)
{
    DlgCashinOut co(u);
    return co.exec();
}

DlgCashinOut::~DlgCashinOut()
{
    delete ui;
}

void DlgCashinOut::on_btnCancel_clicked()
{
    reject();
}

void DlgCashinOut::on_btnCloseSession_clicked()
{
    if (fBalance < 0) {
        C5Message::error(tr("Incorrect balance"));
        return;
    }
    if (C5Message::question(tr("Confirm to close session")) != QDialog::Accepted) {
        return;
    }

    C5Database db(__c5config.dbParams());
    db[":f_id"] = __c5config.getRegValue("session");
    db[":f_dateout"] = QDate::currentDate();
    db[":f_timeout"] = QTime::currentTime();
    db.exec("update s_salary_inout set f_dateout=:f_dateout, f_timeout=:f_timeout where f_id=:f_id");
    db.commit();

    db[":f_session"] = __c5config.getRegValue("session");
    QString sql = "SELECT SUM(oh.f_amountcash) AS f_cash, SUM(oh.f_amountcard) AS f_card, SUM(oh.f_amountidram) AS f_idram "
                  "from s_salary_inout s "
                  "INNER JOIN a_header_cash hc ON hc.f_session=s.f_id "
                  "INNER JOIN o_header oh ON oh.f_id=hc.f_oheader "
                  "INNER JOIN s_user u ON u.f_id=s.f_user "
                  "WHERE f_session=:f_session ";
    db.exec(sql);
    db.nextRow();
    double cash = db.getDouble("f_cash"),
            card = db.getDouble("f_card"),
            idram = db.getDouble("f_idram");

    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    QString cashdoc;
    if (!dw.writeAHeader(cashdoc, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH,
                         fUser->id(), QDate::currentDate(), QDate::currentDate(), QTime::currentTime(),
                         0, fBalance, tr("Close session"))) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    if (!dw.writeAHeaderCash(cashdoc, 0, __c5config.cashId(), 0, "", "", __c5config.getRegValue("session").toInt())) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdoc, __c5config.cashId(), -1, tr("Close session"), fBalance, cashUUID, 1)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    db[":f_id"] = __c5config.getRegValue("session");
    db.exec("select * from s_salary_inout where f_id=:f_id");
    db.nextRow();
    QString datein = db.getDate("f_datein").toString(FORMAT_DATE_TO_STR);
    QString dateout = db.getDate("f_dateout").toString(FORMAT_DATE_TO_STR);
    QString timein = db.getTime("f_timein").toString(FORMAT_TIME_TO_STR);
    QString timeout = db.getTime("f_timeout").toString(FORMAT_TIME_TO_STR);

    QFont font(qApp->font());
    font.setPointSize(__c5config.getValue(param_receipt_print_font_size).toInt());
    font.setFamily(__c5config.getValue(param_receipt_print_font_family));
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(QString("%1 %2").arg(tr("Close session"), __c5config.getRegValue("sessoin").toString()));
    p.br();
    p.ctext(fUser->fullName());
    p.br();
    p.ctext(QString("%1 %2").arg(datein, timein));
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(QString("%1 %2").arg(dateout, timeout));
    p.br();
    p.line();
    p.br();

    p.ltext(ui->tbl->item(0, 0)->text(), 0);
    p.rtext(ui->tbl->item(0, 1)->text());
    p.br();
    p.br();
    p.ltext(tr("Including:"), 0);
    p.br();
    if (cash > 0.001) {
        p.ltext(tr("Cash"), 0);
        p.rtext(float_str(cash, 2));
        p.br();
    }
    if (card > 0.001) {
        p.ltext(tr("Card"), 0);
        p.rtext(float_str(card, 2));
        p.br();
    }if (idram > 0.001) {
        p.ltext(tr("Idram"), 0);
        p.rtext(float_str(idram, 2));
        p.br();
    }
    p.br();
    p.line();
    p.br();

    p.ltext(tr("Other transactions"), 0);
    p.br();
    p.br();
    for (int i = 1 ; i < ui->tbl->rowCount(); i++) {
        cash += str_float(ui->tbl->item(i, 1)->text());
        p.ltext(ui->tbl->item(i, 0)->text(), 0);
        p.rtext(ui->tbl->item(i, 1)->text());
        p.br();
    }
    p.line();
    p.br();
    p.br();
    p.ltext(tr("Total balance"), 0);
    p.rtext(ui->lbBalance->text());
    p.br();
    p.ltext(tr("Avaiable cash"), 0);
    p.rtext(float_str(cash, 2));
    p.br();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    accept();
}
