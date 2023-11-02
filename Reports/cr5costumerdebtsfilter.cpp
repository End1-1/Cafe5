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
    ui->leHall->setSelector(dbParams, ui->leHallName ,cache_halls);
    ui->leManager->setSelector(dbParams, ui->leManagerName, cache_users);
}

CR5CostumerDebtsFilter::~CR5CostumerDebtsFilter()
{
    delete ui;
}

QString CR5CostumerDebtsFilter::condition()
{
    if (ui->rbCommonView->isChecked() || ui->rbTAccount->isChecked()) {
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
    if (ui->rbDebtsofCustomer->isChecked()) {
        if (!ui->leHall->text().isEmpty()) {
            cond += QString(" and cd.f_flag in (%1) ").arg(ui->leHall->text());
        }
        if (!ui->leManager->text().isEmpty()) {
            cond += QString(" and p.f_manager in (%1)").arg(ui->leManager->text());
        }
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

QString CR5CostumerDebtsFilter::sourceCond()
{
    return QString(" and cd.f_source=%1 ").arg(ui->rbDebtsToPartner->isChecked() ? BCLIENTDEBTS_SOURCE_INPUT : BCLIENTDEBTS_SOURCE_SALE);
}

QString CR5CostumerDebtsFilter::flagCond()
{
    if (ui->rbDebtsofCustomer->isChecked()) {
        if (!ui->leHall->text().isEmpty()) {
            return QString(" and cd.f_flag in (%1) ").arg(ui->leHall->text());
        }
        if (!ui->leManager->text().isEmpty()) {
            return QString(" and p.f_manager in (%1)").arg(ui->leManager->text());
        }
    }
    return "";
}

int CR5CostumerDebtsFilter::viewMode()
{
    if (ui->rbTransaction->isChecked()) {
        return 1;
    }
    if (ui->rbCommonView->isChecked()) {
        return 2;
    }
    if (ui->rbTAccount->isChecked()) {
        return 3;
    }
}

QString CR5CostumerDebtsFilter::date1()
{
    return ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QString CR5CostumerDebtsFilter::date2()
{
    return ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QString CR5CostumerDebtsFilter::manager()
{
    return ui->leManager->text();
}

QString CR5CostumerDebtsFilter::flag()
{
    return ui->leHall->text();
}

bool CR5CostumerDebtsFilter::debtMode()
{
    return ui->rbDebtsToPartner->isChecked();
}
