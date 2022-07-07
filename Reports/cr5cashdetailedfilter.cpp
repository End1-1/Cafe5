#include "cr5cashdetailedfilter.h"
#include "ui_cr5cashdetailedfilter.h"
#include "c5cache.h"

CR5CashDetailedFilter::CR5CashDetailedFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CashDetailedFilter)
{
    ui->setupUi(this);
    ui->leCash->setSelector(dbParams, ui->leCashName, cache_cash_names);
}

CR5CashDetailedFilter::~CR5CashDetailedFilter()
{
    delete ui;
}

QString CR5CashDetailedFilter::condition()
{
    return "";
}

QString CR5CashDetailedFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leCashName);
    return s;
}

QDate CR5CashDetailedFilter::date1() const
{
    return ui->deStart->date();
}

QDate CR5CashDetailedFilter::date2() const
{
    return ui->deEnd->date();
}

int CR5CashDetailedFilter::cash()
{
    return ui->leCash->getInteger();
}
