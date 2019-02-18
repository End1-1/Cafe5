#include "cr5salefromstorefilter.h"
#include "ui_cr5salefromstorefilter.h"

CR5SaleFromStoreFilter::CR5SaleFromStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleFromStoreFilter)
{
    ui->setupUi(this);
}

CR5SaleFromStoreFilter::~CR5SaleFromStoreFilter()
{
    delete ui;
}

QString CR5SaleFromStoreFilter::condition()
{
    return " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
}

QDate CR5SaleFromStoreFilter::d1()
{
    return ui->deStart->date();
}

QDate CR5SaleFromStoreFilter::d2()
{
    return ui->deEnd->date();
}
