#include "cr5discountstatisticsfilter.h"
#include "ui_cr5discountstatisticsfilter.h"
#include "cachediscounttype.h"

CR5DiscountStatisticsFilter::CR5DiscountStatisticsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5DiscountStatisticsFilter)
{
    ui->setupUi(this);
    ui->leDisc->setSelector(dbParams, ui->leDiscName, cache_discount_type);
}

CR5DiscountStatisticsFilter::~CR5DiscountStatisticsFilter()
{
    delete ui;
}

QString CR5DiscountStatisticsFilter::condition()
{
    QString cond = QString(" oh.f_datecash between %1 and %2 ").arg(ui->deStart->toMySQLDate()).arg(ui->deEnd->toMySQLDate());
    if (ui->leDisc->getInteger() > 0) {
        cond += " and bd.f_mode=" + ui->leDisc->text() + " ";
    }
    return cond;
}
