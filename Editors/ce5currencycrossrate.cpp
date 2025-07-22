#include "ce5currencycrossrate.h"
#include "ui_ce5currencycrossrate.h"
#include "c5cache.h"
#include <QDoubleValidator>

CE5CurrencyCrossRate::CE5CurrencyCrossRate(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5CurrencyCrossRate)
{
    ui->setupUi(this);
    ui->leBaseCurr->setSelector(ui->leBaseCurrName, cache_currency);
    ui->leDepCurr->setSelector(ui->leDepCurrName, cache_currency);
    ui->leRate->setValidator(new QDoubleValidator(0, 99999999, 7));
    ui->leRate->fDecimalPlaces = 4;
}

CE5CurrencyCrossRate::~CE5CurrencyCrossRate()
{
    delete ui;
}

QString CE5CurrencyCrossRate::title()
{
    return tr("Cross currency rate");
}

QString CE5CurrencyCrossRate::table()
{
    return "e_currency_cross_rate";
}
