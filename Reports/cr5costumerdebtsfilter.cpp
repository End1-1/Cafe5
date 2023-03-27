#include "cr5costumerdebtsfilter.h"
#include "ui_cr5costumerdebtsfilter.h"
#include "c5cache.h"
#include "bclientdebts.h"

CR5CostumerDebtsFilter::CR5CostumerDebtsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CostumerDebtsFilter)
{
    ui->setupUi(this);
    ui->leCostumer->setSelector(dbParams, ui->leCostumerName, cache_goods_partners);
    ui->deStart->setDate(QDate::currentDate().addDays(-1 * 90));
}

CR5CostumerDebtsFilter::~CR5CostumerDebtsFilter()
{
    delete ui;
}

QString CR5CostumerDebtsFilter::condition()
{
    if (ui->chShowTotal->isChecked()) {
        return "";
    }
    QString cond = QString("where cd.f_date between '%1' and '%2' ").arg(date1(), date2());
    if (!ui->leCostumer->isEmpty()) {
        cond += " and cd.f_costumer in (" + ui->leCostumer->text() + ") ";
    }
    if (ui->rbDebts->isChecked()) {
        cond += " and cd.f_amount<0 ";
    }
    if (ui->rbPaid->isChecked()) {
        cond += " and cd.f_amount>0 ";
    }
    cond += QString(" and cd.f_source=%1 ").arg(ui->rbDebtsToPartner->isChecked() ? BCLIENTDEBTS_SOURCE_INPUT : BCLIENTDEBTS_SOURCE_SALE);
    return cond;
}

QString CR5CostumerDebtsFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leCostumerName);
    return s;
}

bool CR5CostumerDebtsFilter::isTotal()
{
    return ui->chShowTotal->isChecked();
}

QString CR5CostumerDebtsFilter::date1()
{
    return ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QString CR5CostumerDebtsFilter::date2()
{
    return ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

bool CR5CostumerDebtsFilter::debtMode()
{
    return ui->rbDebtsToPartner->isChecked();
}
