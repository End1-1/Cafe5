#include "cr5salefromstoretotalfilter.h"
#include "ui_cr5salefromstoretotalfilter.h"
#include "c5cache.h"

CR5SaleFromStoreTotalFilter::CR5SaleFromStoreTotalFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5SaleFromStoreTotalFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store).setMultiselection(true);
}

CR5SaleFromStoreTotalFilter::~CR5SaleFromStoreTotalFilter()
{
    delete ui;
}

QString CR5SaleFromStoreTotalFilter::condition()
{
    return "";
}

QString CR5SaleFromStoreTotalFilter::filterText()
{
    QString cond = QString("%1 %2-%3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(cond, ui->leStoreName);
    return cond;
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
