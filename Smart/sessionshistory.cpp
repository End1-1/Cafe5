#include "sessionshistory.h"
#include "ui_sessionshistory.h"
#include "c5printing.h"
#include <QFile>

SessionsHistory::SessionsHistory(const QDate &d1, const QDate &d2) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::SessionsHistory)
{
    ui->setupUi(this);
    fDate1 = d1;
    fDate2 = d2;
    ui->tbl->setColumnWidth(0, 80);
    ui->tbl->setColumnWidth(1, 200);
    ui->tbl->setColumnWidth(2, 200);
    ui->tbl->setColumnWidth(3, 150);
    C5Database db(fDBParams);
    db[":f_date1"] = fDate1;
    db[":f_date2"] = fDate2;
    db.exec("select s.f_id, s.f_datein, s.f_timein, s.f_dateout, s.f_timeout, sum(e.f_amount) as f_amount "
            "from s_salary_inout s "
            "inner join a_header_cash hc on hc.f_session=s.f_id "
            "inner join e_cash e on e.f_header=hc.f_id "
            "where f_datein between :f_date1 and :f_date2 and length(hc.f_oheader)>0 "
            "group by s.f_id ");
    while (db.nextRow()) {
        int r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.getString("f_id")));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(QString("%1 %2").arg(db.getDate("f_datein").toString(FORMAT_DATE_TO_STR),
                         db.getTime("f_timein").toString(FORMAT_TIME_TO_STR))));
        ui->tbl->setItem(r, 2, new QTableWidgetItem(QString("%1 %2").arg(db.getDate("f_dateout").toString(FORMAT_DATE_TO_STR),
                         db.getTime("f_timeout").toString(FORMAT_TIME_TO_STR))));
        ui->tbl->setItem(r, 3, new QTableWidgetItem(float_str(db.getDouble("f_amount"), 2)));
    }
}

SessionsHistory::~SessionsHistory()
{
    delete ui;
}

void SessionsHistory::on_btnPrintReport_clicked()
{
    QStringList sessions;
    C5Database db(fDBParams);
    db[":f_date1"] = fDate1;
    db[":f_date2"] = fDate2;
    db.exec("select f_id from s_salary_inout where f_datein between :f_date1 and :f_date2 ");
    while (db.nextRow()) {
        sessions.append(db.getString("f_id"));
    }
    QFont font(qApp->font());
    font.setPointSize(__c5config.getValue(param_receipt_print_font_size).toInt());
    font.setFamily(__c5config.getValue(param_receipt_print_font_family));
    C5Printing p;
    p.setSceneParams(650, 2800, QPageLayout::Portrait);
    p.setFont(font);
    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(QString("%1").arg(tr("History of sessions")));
    p.br();
    for (const QString &sid : sessions) {
        db[":f_id"] = sid;
        db.exec("select concat_ws(u.f_last, u.f_first) as f_fullname, s.* "
                "from s_salary_inout s "
                "left join s_user u on u.f_id=s.f_user "
                "where s.f_id=:f_id");
        db.nextRow();
        p.ctext(QString("%1 %2").arg(db.getDate("f_datein").toString(FORMAT_DATE_TO_STR),
                                     db.getTime("f_timein").toString(FORMAT_TIME_TO_SHORT_STR)));
        p.br();
        p.ctext("-");
        p.br();
        p.ctext(QString("%1 %2").arg(db.getDate("f_dateout").toString(FORMAT_DATE_TO_STR),
                                     db.getTime("f_timeout").toString(FORMAT_TIME_TO_SHORT_STR)));
        p.br();
        p.ctext(db.getString("f_fullname"));
        p.br();
        p.line();
        p.br();
        double balance = 0;
        db[":f_session"] = sid;
        //db[":f_cash"] = __c5config.cashId();
        db.exec("select sum(f_amount) as f_amount "
                "from e_cash e "
                "inner join a_header_cash hc on hc.f_id=e.f_header "
                "where length(hc.f_oheader)>0 and f_session=:f_session ");
        db.nextRow();
        balance += db.getDouble("f_amount");
        p.ltext(tr("Input from sale"), 0);
        p.rtext(float_str(db.getDouble("f_amount"), 2));
        p.br();
        db[":f_session"] = sid;
        //db[":f_cash"] = __c5config.cashId();
        db.exec("select f_amount, f_remarks "
                "from e_cash e "
                "inner join a_header_cash hc on hc.f_id=e.f_header "
                "where length(hc.f_oheader)=0 and f_session=:f_session and f_sign=1");
        while (db.nextRow()) {
            p.ltext(db.getString("f_remarks"), 0);
            p.rtext(float_str(db.getDouble("f_amount"), 2));
            p.br();
            balance += db.getDouble("f_amount");
        }
        db[":f_session"] = sid;
        //db[":f_cash"] = __c5config.cashId();
        db.exec("select f_sign*f_amount as f_amount, f_remarks "
                "from e_cash e "
                "inner join a_header_cash hc on hc.f_id=e.f_header "
                "where length(hc.f_oheader)=0 and f_session=:f_session and f_sign=-1");
        while (db.nextRow()) {
            p.ltext(db.getString("f_remarks"), 0);
            p.rtext(float_str(db.getDouble("f_amount"), 2));
            p.br();
            balance += db.getDouble("f_amount");
        }
        p.ltext(tr("Final balance"), 0);
        p.rtext(float_str(balance, 2));
        p.br();
        p.line();
        p.br();
    }
    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
}

void SessionsHistory::on_btnCancel_clicked()
{
    reject();
}

void SessionsHistory::on_btnPrintSession_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    QString session = ui->tbl->item(r, 0)->text();
    C5Database db(__c5config.dbParams());
    db[":f_id"] = session;
    db.exec("select concat_ws(' ', u.f_last, u.f_first) as f_fullname, s.* "
            "from s_salary_inout s "
            "left join s_user u on u.f_id=s.f_user "
            "where s.f_id=:f_id");
    db.nextRow();
    QString username = db.getString("f_fullname");
    db[":f_session"] = session;
    QString sql =
        "SELECT SUM(oh.f_amountcash) AS f_cash, SUM(oh.f_amountcard) AS f_card, SUM(oh.f_amountidram) AS f_idram, "
        "concat_ws(' ', u.f_last, u.f_first) as f_fullname "
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
    QFont font(qApp->font());
    font.setPointSize(__c5config.getValue(param_receipt_print_font_size).toInt());
    font.setFamily(__c5config.getValue(param_receipt_print_font_family));
    C5Printing p;
    p.setSceneParams(650, 2800, QPageLayout::Portrait);
    p.setFont(font);
    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(QString("%1 %2").arg(tr("Close session"), __c5config.getRegValue("session").toString()));
    p.br();
    p.ctext(username);
    p.br();
    p.ctext(ui->tbl->item(r, 1)->text());
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(ui->tbl->item(r, 2)->text());
    p.br();
    p.line();
    p.br();
    p.ltext(tr("Total revenue"), 0);
    p.rtext(float_str(cash + card + idram, 2));
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
    p.br();
    p.line();
    p.br();
    p.ltext(tr("Other transactions"), 0);
    p.br();
    p.br();
    db[":f_session"] = session;
    db.exec("select f_sign*f_amount as f_amount, f_remarks "
            "from e_cash e "
            "inner join a_header_cash hc on hc.f_id=e.f_header "
            "where length(hc.f_oheader)=0 and f_session=:f_session and f_sign=-1");
    while (db.nextRow()) {
        p.ltext(db.getString("f_remarks"), 0);
        p.rtext(float_str(db.getDouble("f_amount"), 2));
        p.br();
    }
    p.line();
    p.br();
    p.br();
    p.ltext(tr("Total balance"), 0);
    p.rtext("0");
    p.br();
    p.br();
    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
}
