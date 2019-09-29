#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"
#include "c5cache.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_order_state);
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    QString result = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
    if (!ui->leState->isEmpty()) {
        result += " and oh.f_state in (" + ui->leState->text() + ") ";
    }
    return result;
}
