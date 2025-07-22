#include "cr5currencyratehistoryfilter.h"
#include "ui_cr5currencyratehistoryfilter.h"

CR5CurrencyRateHistoryFilter::CR5CurrencyRateHistoryFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CurrencyRateHistoryFilter)
{
    ui->setupUi(this);
}

CR5CurrencyRateHistoryFilter::~CR5CurrencyRateHistoryFilter()
{
    delete ui;
}

QString CR5CurrencyRateHistoryFilter::condition()
{
    QString where = QString(" h.f_date between %1 and %2").arg(ui->leD1->toMySQLDate(), ui->leD2->toMySQLDate());
    in(where, "h.f_currency", ui->leCurrency);
    return where;
}
