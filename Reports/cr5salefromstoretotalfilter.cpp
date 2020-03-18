#include "cr5salefromstoretotalfilter.h"
#include "ui_cr5salefromstoretotalfilter.h"
#include "c5cache.h"

CR5SaleFromStoreTotalFilter::CR5SaleFromStoreTotalFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleFromStoreTotalFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
}

CR5SaleFromStoreTotalFilter::~CR5SaleFromStoreTotalFilter()
{
    delete ui;
}

QString CR5SaleFromStoreTotalFilter::condition()
{
    return "";
}

int CR5SaleFromStoreTotalFilter::store()
{
    return ui->leStore->getInteger();
}

QDate CR5SaleFromStoreTotalFilter::start() const
{
    return ui->deStart->date();
}

QDate CR5SaleFromStoreTotalFilter::end() const
{
    return ui->deEnd->date();
}

bool CR5SaleFromStoreTotalFilter::showQty()
{
    return ui->chShowQty->isChecked();
}

bool CR5SaleFromStoreTotalFilter::showAmount()
{
    return ui->chShowAmount->isChecked();
}
