#include "dlgviewcashreport.h"
#include "ui_dlgviewcashreport.h"

DlgViewCashReport::DlgViewCashReport() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgViewCashReport)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 300);
}

DlgViewCashReport::~DlgViewCashReport()
{
    delete ui;
}

void DlgViewCashReport::addTotal(double t)
{
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(tr("Total")));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(t, 2)));
    r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setSpan(r, 0, 1, 2);
}

void DlgViewCashReport::addTitle(const QString &t)
{
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setSpan(r, 0, 1, 2);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(t));
}

void DlgViewCashReport::addRow(const QString &t, double v)
{
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(t));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(float_str(v, 2)));
}

void DlgViewCashReport::on_btnClose_clicked()
{
    reject();
}

void DlgViewCashReport::on_btnPrint_clicked()
{
    accept();
}
