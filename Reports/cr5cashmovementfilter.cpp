#include "cr5cashmovementfilter.h"
#include "ui_cr5cashmovementfilter.h"
#include "c5cache.h"

CR5CashMovementFilter::CR5CashMovementFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CashMovementFilter)
{
    ui->setupUi(this);
    ui->leCash->setSelector(ui->leCashName, cache_cash_names).setMultiselection(true);
    ui->leHall->setSelector(ui->leHallname, cache_halls);
    ui->leManager->setSelector(ui->leManagerName, cache_users);
}

CR5CashMovementFilter::~CR5CashMovementFilter()
{
    delete ui;
}

QString CR5CashMovementFilter::condition()
{
    QString w = " h.f_state=1 ";
    w += QString(" and h.f_date between %1 and %2 ").arg(ui->deStart->toMySQLDate()).arg(ui->deEnd->toMySQLDate());
    if (!ui->leCash->isEmpty()) {
        w += " and ec.f_cash in (" + ui->leCash->text() + ") ";
    }
    if (ui->leHall->isEmpty() == false) {
        w += " and cd.f_flag in (" + ui->leHall->text() + ") ";
    }
    if (ui->leManager->isEmpty() == false) {
        w += " and p.f_manager in (" + ui->leManager->text() + ") ";
    }
    return w;
}

QString CR5CashMovementFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leCashName);
    return s;
}
