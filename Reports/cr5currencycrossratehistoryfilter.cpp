#include "cr5currencycrossratehistoryfilter.h"
#include "ui_cr5currencycrossratehistoryfilter.h"
#include "c5cache.h"

CR5CurrencyCrossRateHistoryFilter::CR5CurrencyCrossRateHistoryFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CurrencyCrossRateHistoryFilter)
{
    ui->setupUi(this);
    ui->leCurrency->setSelector(ui->leCurrencyName, cache_currency);
    ui->leCurrency_2->setSelector(ui->leCurrencyName_2, cache_currency);
}

CR5CurrencyCrossRateHistoryFilter::~CR5CurrencyCrossRateHistoryFilter()
{
    delete ui;
}

QString CR5CurrencyCrossRateHistoryFilter::condition()
{
    QString where = QString(" cr.f_date between %1 and %2 ").arg(ui->leD1->toMySQLDate(), ui->leD2->toMySQLDate());
    in(where, "cr.f_currency1", ui->leCurrency);
    in(where, "cr.f_currency2", ui->leCurrency_2);
    return where;
}
