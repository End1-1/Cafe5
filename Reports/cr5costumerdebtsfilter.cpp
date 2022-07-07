#include "cr5costumerdebtsfilter.h"
#include "ui_cr5costumerdebtsfilter.h"
#include "c5cache.h"

CR5CostumerDebtsFilter::CR5CostumerDebtsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CostumerDebtsFilter)
{
    ui->setupUi(this);
    ui->leCostumer->setSelector(dbParams, ui->leCostumerName, cache_discount_client);
    ui->deStart->setDate(QDate::currentDate().addDays(-1 * 90));
}

CR5CostumerDebtsFilter::~CR5CostumerDebtsFilter()
{
    delete ui;
}

QString CR5CostumerDebtsFilter::condition()
{
    QString cond;
    if (ui->chShowTotal->isChecked()) {
        cond = QString(" cd.f_date between '%1' and '%2'")
            .arg(QDate::fromString("2000-01-01", FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR_MYSQL), QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL));
    } else {
        cond = QString(" cd.f_date between %1 and %2")
            .arg(ui->deStart->toMySQLDate(), ui->deEnd->toMySQLDate());
    }
    if (!ui->leCostumer->isEmpty()) {
        cond += " and cd.f_costumer in (" + ui->leCostumer->text() + ") ";
    }
    if (ui->rbDebts->isChecked()) {
        cond += " and cd.f_amount<0 ";
    }
    if (ui->rbPaid->isChecked()) {
        cond += " and cd.f_amount>0 ";
    }
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
