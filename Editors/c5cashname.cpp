#include "c5cashname.h"
#include "ui_c5cashname.h"
#include "c5cache.h"

C5CashName::C5CashName(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::C5CashName)
{
    ui->setupUi(this);
    ui->leCurr->setSelector(ui->leCurrName, cache_currency);
}

C5CashName::~C5CashName()
{
    delete ui;
}

QString C5CashName::title()
{
    return tr("Cashbox");
}

QString C5CashName::table()
{
    return "cash_box";
}
