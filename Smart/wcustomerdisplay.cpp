#include "wcustomerdisplay.h"
#include "ui_wcustomerdisplay.h"

WCustomerDisplay::WCustomerDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WCustomerDisplay)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 400);
    ui->tbl->setColumnWidth(1, 150);
    ui->tbl->setColumnWidth(2, 150);
    ui->tbl->setColumnWidth(3, 250);
    ui->tbl->horizontalHeader()->setProperty("bold", "2");
    ui->tbl->verticalHeader()->setProperty("bold", "2");
    ui->tbl->horizontalHeader()->style()->polish(ui->tbl->horizontalHeader());
}

WCustomerDisplay::~WCustomerDisplay()
{
    delete ui;
}

void WCustomerDisplay::clear()
{
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    ui->lbAmount->setText(0);
}

void WCustomerDisplay::addRow(const QString &name, const QString &qty, const QString &price, const QString &total)
{
    int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(name));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(qty));
    ui->tbl->setItem(r, 2, new QTableWidgetItem(price));
    ui->tbl->setItem(r, 3, new QTableWidgetItem(total));
}

void WCustomerDisplay::setTotal(const QString &total)
{
    QStringList l;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        l.append(QString::number(i + 1));
    }
    ui->tbl->setVerticalHeaderLabels(l);
    ui->tbl->resizeRowsToContents();
    ui->lbAmount->setText(total);
}
