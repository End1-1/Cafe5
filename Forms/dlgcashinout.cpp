#include "dlgcashinout.h"
#include "ui_dlgcashinout.h"
#include "c5printing.h"
#include "c5user.h"
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
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    //db[":f_cash"] = __c5config.cashId();
    db.exec("select sum(f_amounttotal) as f_amounttotal, sum(f_amountcash) as f_amountcash "
            "from o_header "
            "where f_working_session=:f_working_session and f_state=2 ");
    db.nextRow();
    balance += db.getDouble("f_amounttotal");
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(tr("Input from sale")));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amounttotal"), 2)));
    r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(tr("Cash amount")));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amountcash"), 2)));
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    db.exec("select sum(f_amounttotal) as f_amount "
            "from o_header oh "
            "inner join o_header_flags ohf on ohf.f_id=oh.f_id "
            "where oh.f_working_session=:f_working_session and ohf.f_1=1");
    db.nextRow();
    //balance += db.getDouble("f_amount");
    r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(tr("Input from delivery")));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    db.exec("select f_amount, f_remarks "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(coalesce(hc.f_oheader, ''))=0 and f_working_session=:f_working_session and f_sign=1");
    while (db.nextRow()) {
        r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.getString("f_remarks")));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));
        balance += db.getDouble("f_amount");
    }
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    db.exec("select f_sign*f_amount as f_amount, f_remarks "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(coalesce(hc.f_oheader, ''))=0 and f_working_session=:f_working_session and f_sign=-1");
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
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    db[":f_close"] = QDateTime::currentDateTime();
    db.exec("update s_working_sessions "
            "set f_close=:f_close where f_id=:f_working_session ");
    db.commit();
    db[":f_working_session"] = __c5config.getRegValue("sessionid");
    QString sql = "SELECT SUM(oh.f_amountcash) AS f_cash, SUM(oh.f_amountcard) AS f_card, "
                  "SUM(oh.f_amountidram) AS f_idram, sum(oh.f_amountother) as f_amountother "
                  "from o_header oh "
                  "WHERE oh.f_working_session=:f_working_session ";
    db.exec(sql);
    db.nextRow();
    double cash = db.getDouble("f_cash"),
           card = db.getDouble("f_card"),
           idram = db.getDouble("f_idram"),
           other = db.getDouble("f_amountother");
    db[":f_id"] = __c5config.getRegValue("sessionid");
    db.exec("select sws.*, concat(u.f_last, ' ', u.f_first) as f_fullname "
            "from s_working_sessions sws "
            "left join s_user u on u.f_id=sws.f_user "
            "where sws.f_id=:f_id ");
    if (db.nextRow() == false) {
        C5Message::error(QString("No session with %1").arg(__c5config.getRegValue("sessionid").toInt()));
        return;
    }
    QDateTime open = db.getDateTime("f_open");
    QDateTime close = db.getDateTime("f_close");
    QString staff = db.getString("f_fullname");
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
    p.ctext(QString("%1 #%2").arg(tr("Close session"), QString::number(db.getInt("f_id"))));
    p.br();
    p.ctext(staff);
    p.br();
    p.ctext(QString("%1").arg(open.toString(FORMAT_DATETIME_TO_STR)));
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(QString("%1").arg(close.toString(FORMAT_DATETIME_TO_STR)));
    p.br();
    p.line();
    p.br();
    p.ltext(ui->tbl->item(0, 0)->text(), 0);
    p.rtext(ui->tbl->item(0, 1)->text());
    // if (str_float(ui->tbl->item(1, 1)->text()) > 0.01) {
    //     p.br();
    //     p.ltext(ui->tbl->item(1, 0)->text(), 0);
    //     p.rtext(ui->tbl->item(1, 1)->text());
    // }
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
    }
    if (idram > 0.001) {
        p.ltext(tr("Idram"), 0);
        p.rtext(float_str(idram, 2));
        p.br();
    }
    if (other > 0.001) {
        p.ltext(tr("Other"), 0);
        p.rtext(float_str(other, 2));
        p.br();
    }
    p.br();
    p.line();
    p.br();
    p.ltext(tr("Other transactions"), 0);
    p.br();
    p.br();
    for (int i = 3 ; i < ui->tbl->rowCount(); i++) {
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
