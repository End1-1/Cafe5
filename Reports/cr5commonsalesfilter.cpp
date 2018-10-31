#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    return " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
}
