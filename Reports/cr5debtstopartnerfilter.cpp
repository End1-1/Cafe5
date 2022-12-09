#include "cr5debtstopartnerfilter.h"
#include "ui_cr5debtstopartnerfilter.h"
#include "c5cache.h"

CR5DebtsToPartnerFilter::CR5DebtsToPartnerFilter(const QStringList &dbParams) :
    C5FilterWidget(dbParams),
    ui(new Ui::CR5DebtsToPartnerFilter)
{
    ui->setupUi(this);
    ui->leCurrency->setSelector(dbParams, ui->leCurrencyName, cache_currency);
}

CR5DebtsToPartnerFilter::~CR5DebtsToPartnerFilter()
{
    delete ui;
}

QString CR5DebtsToPartnerFilter::condition()
{
    return "";
}

QDate CR5DebtsToPartnerFilter::d1() const
{
    return ui->deStart->date();
}

QDate CR5DebtsToPartnerFilter::d2() const
{
    return ui->deEnd->date();
}

QString CR5DebtsToPartnerFilter::currency() const
{
    return ui->leCurrency->text();
}
