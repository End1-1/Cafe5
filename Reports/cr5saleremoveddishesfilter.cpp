#include "cr5saleremoveddishesfilter.h"
#include "ui_cr5saleremoveddishesfilter.h"
#include "c5cache.h"

CR5SaleRemovedDishesFilter::CR5SaleRemovedDishesFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5SaleRemovedDishesFilter)
{
    ui->setupUi(this);
    ui->leDishState->setSelector(ui->leStateName, cache_dish_state);
}

CR5SaleRemovedDishesFilter::~CR5SaleRemovedDishesFilter()
{
    delete ui;
}

QString CR5SaleRemovedDishesFilter::condition()
{
    QString result = " (oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " or oh.f_datecash is null)";
    if (ui->leDishState->isEmpty()) {
        result += " and ob.f_state in (2,3)";
    } else {
        result += " and ob.f_state in (" + ui->leDishState->text() + ")";
    }
    return result;
}

QString CR5SaleRemovedDishesFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leStateName);
    return s;
}
