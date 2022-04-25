#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "dlgreportslist.h"
#include "c5printjson.h"
#include "c5translator.h"
#include "c5cafecommon.h"
#include "c5user.h"
#include "datadriver.h"
#include "dlglistofshifts.h"
#include "dlgreceiptlanguage.h"
#include "dlglistofhall.h"
#include "printtaxn.h"

DlgReports::DlgReports(const QStringList &dbParams, C5User *user) :
    C5Dialog(dbParams),
    ui(new Ui::DlgReports),
    fUser(user)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    ui->date1->setDate(QDate::currentDate());
    ui->date2->setDate(QDate::currentDate());
    fShiftId = 0;
    setLangIcon();
}

DlgReports::~DlgReports()
{
    delete ui;
}

void DlgReports::openReports(C5User *user)
{
    DlgReports *d = new DlgReports(QStringList(), user);
    d->getDailyCommon();
    d->exec();
    delete d;
}

void DlgReports::getDailyCommon(const QDate &date1, const QDate &date2)
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleDailyCommon(QJsonObject)));
    sh->bind("cmd", sm_dailycommon);
    sh->bind("date1", date1.toString(FORMAT_DATE_TO_STR_MYSQL));
    sh->bind("date2", date2.toString(FORMAT_DATE_TO_STR_MYSQL));
    sh->bind("hall", QString::number(fCurrentHall));
    sh->bind("shift", QString::number(fShiftId));
    sh->bind("cash", QString::number(__c5config.cashId()));
    sh->send();
}

void DlgReports::setLangIcon()
{
    switch (C5Config::getRegValue("receipt_language").toInt()) {
    case 0:
        ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
        C5Config::setRegValue("receipt_language", LANG_AM);
        break;
    case 1:
        ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
        C5Config::setRegValue("receipt_language", LANG_EN);
        break;
    case 2:
        ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
        C5Config::setRegValue("receipt_language", LANG_RU);
        break;
    }
}

void DlgReports::handleDailyCommon(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QJsonArray ja = obj["report"].toArray();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    double total = 0;
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, o["f_id"].toString());
        ui->tbl->setString(r, 1, o["f_order"].toString());
        ui->tbl->setString(r, 2, o["f_datecash"].toString());
        ui->tbl->setString(r, 3, o["f_timeclose"].toString());
        ui->tbl->setString(r, 4, o["f_hall"].toString());
        ui->tbl->setString(r, 5, o["f_table"].toString());
        ui->tbl->setString(r, 6, o["f_staff"].toString());
        ui->tbl->setString(r, 7, float_str(o["f_amounttotal"].toString().toDouble(), 2));
        if (__c5config.carMode()) {
            ui->tbl->setString(r, 8, o["f_govnumber"].toString());
        }
        ui->tbl->setString(r, 8, o["f_receiptnumber"].toString());
        total += o["f_amounttotal"].toString().toDouble();
    }
    ui->tblTotal->setColumnCount(ui->tbl->columnCount());
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
    for (int i = 0; i < ui->tbl->columnCount(); i++) {
        ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
    }
    ui->tblTotal->setString(0, 7, float_str(total, 2));
    QStringList l;
    l.append(QString::number(ui->tbl->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);
}

void DlgReports::handleReportsList(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QString file;
    if (!DlgReportsList::getReport(obj["reports"].toArray(), file)) {
        return;
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintReport(QJsonObject)));
    sh->bind("cmd", sm_waiter_report);
    sh->bind("file", file);
    sh->bind("date1", ui->date1->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    sh->bind("date2", ui->date2->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    sh->bind("shift", QString::number(fShiftId));
    sh->bind("paper_width", __c5config.getValue(param_print_paper_width).toInt() == 0 ? 650 : __c5config.getValue(param_print_paper_width).toInt());
    sh->bind("receipt_printer", __c5config.fSettingsName);
    sh->send();
}

void DlgReports::handlePrintReport(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    C5PrintJson *pj = new C5PrintJson(obj["report"].toArray());
    pj->start();
}

void DlgReports::handleReceipt(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
    }
}

void DlgReports::handleTaxback(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    C5Message::info(obj["msg"].toString());
}

void DlgReports::handleTaxReport(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    } else {
        C5Message::info(tr("Printed"));
    }
}

void DlgReports::on_btnRefresh_clicked()
{
    getDailyCommon(ui->date1->date(), ui->date2->date());
}

void DlgReports::on_btnDateLeft1_clicked()
{
    ui->date1->setDate(ui->date1->date().addDays(-1));
}

void DlgReports::on_btnDateLeft2_clicked()
{
    ui->date2->setDate(ui->date2->date().addDays(-1));
}

void DlgReports::on_btnDateRight1_clicked()
{
    ui->date1->setDate(ui->date1->date().addDays(1));
}

void DlgReports::on_btnDateRight2_clicked()
{
    ui->date2->setDate(ui->date2->date().addDays(1));
}

void DlgReports::on_btnExit_clicked()
{
    accept();
}

void DlgReports::on_btnReports_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReportsList(QJsonObject)));
    sh->bind("cmd", sm_reports);
    sh->send();
}

void DlgReports::on_btnPrintOrderReceipt_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    QString orderid = ui->tbl->getString(l.at(0).row(), 0);
    C5Database db(__c5config.dbParams());
    db[":f_id"] = orderid;
    db.exec("select * from o_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid order id"));
        return;
    }
    int tax = 0;
    if (db.getDouble("f_amounttotal") - db.getDouble("f_amountcash") < 0.001) {
        db[":f_id"] = orderid;
        db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt("f_receiptnumber") == 0) {
                tax = C5Message::question(tr("Do you want to print fiscal receipt?")) == QDialog::Accepted ? 1 : 0;
            }
        } else {
            tax = C5Message::question(tr("Do you want to print fiscal receipt?")) == QDialog::Accepted ? 1 : 0;
        }
    } else if (db.getDouble("f_amountcard") > 0.001) {
        tax = 1;
    }

    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_printreceipt);
    sh->bind("staffid", QString::number(fUser->id()));
    sh->bind("staffname", fUser->fullName());
    sh->bind("f_receiptlanguage", QString::number(C5Config::getRegValue("receipt_language").toInt()));
    sh->bind("order", orderid);
    sh->bind("printtax", tax);
    sh->bind("receipt_printer", C5Config::fSettingsName);
    sh->send();
}

void DlgReports::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage();
    if (r > -1) {
        C5Config::setRegValue("receipt_language", r);
        setLangIcon();
    }
}

void DlgReports::on_btnHall_clicked()
{
    int hall;
    if (!DlgListOfHall::getHall(hall)) {
        return;
    }
    fCurrentHall = hall;
    ui->btnHall->setText(dbhall->name(fCurrentHall));
}

void DlgReports::on_btnShift_clicked()
{
    if (DlgListOfShifts::getShift(fShiftId, fShiftName)) {
        ui->btnShift->setText(fShiftName);
    }
}

void DlgReports::on_btnReturnTaxReceipt_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    QString orderid = ui->tbl->getString(l.at(0).row(), 0);
    C5SocketHandler *sh = createSocketHandler(SLOT(handleTaxback(QJsonObject)));
    sh->bind("cmd", sm_retrun_tax_receipt);
    sh->bind("order", orderid);
    sh->send();
}

void DlgReports::on_btnPrintTaxX_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleTaxReport(QJsonObject)));
    sh->bind("cmd", sm_tax_report);
    sh->bind("d1", ui->date1->date().toString(FORMAT_DATE_TO_STR));
    sh->bind("d2", ui->date1->date().toString(FORMAT_DATE_TO_STR));
    sh->bind("type", report_x);
    sh->send();
}

void DlgReports::on_btnPrintTaxZ_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleTaxReport(QJsonObject)));
    sh->bind("cmd", sm_tax_report);
    sh->bind("d1", ui->date1->date().toString(FORMAT_DATE_TO_STR));
    sh->bind("d2", ui->date1->date().toString(FORMAT_DATE_TO_STR));
    sh->bind("type", report_z);
    sh->send();
}

