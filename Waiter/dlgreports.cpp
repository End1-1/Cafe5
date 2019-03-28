#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "dlgreportslist.h"
#include "c5printjson.h"

DlgReports::DlgReports(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgReports)
{
    ui->setupUi(this);
    ui->date1->setDate(QDate::currentDate());
    ui->date2->setDate(QDate::currentDate());

}

DlgReports::~DlgReports()
{
    delete ui;
}

void DlgReports::openReports(C5User *user)
{
    DlgReports *d = new DlgReports(QStringList());
    d->fUser = user;
    d->showFullScreen();
    d->hide();
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
    sh->send();
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
    }
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
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
    ui->date2->setDate(ui->date2->date().addDays(-1));
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
