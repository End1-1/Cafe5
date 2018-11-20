#include "cr5consumptionbysalesfilter.h"
#include "ui_cr5consumptionbysalesfilter.h"
#include "c5cache.h"

CR5ConsumptionBySalesFilter::CR5ConsumptionBySalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ConsumptionBySalesFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
}

CR5ConsumptionBySalesFilter::~CR5ConsumptionBySalesFilter()
{
    delete ui;
}

QString CR5ConsumptionBySalesFilter::condition()
{
    return "";
}

int CR5ConsumptionBySalesFilter::store()
{
    return ui->leStore->getInteger();
}

QDate CR5ConsumptionBySalesFilter::date1()
{
    return ui->deStart->date();
}

QDate CR5ConsumptionBySalesFilter::date2()
{
    return ui->deEnd->date();
}
