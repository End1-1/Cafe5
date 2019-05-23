#include "cr5saleremoveddishesfilter.h"
#include "ui_cr5saleremoveddishesfilter.h"

CR5SaleRemovedDishesFilter::CR5SaleRemovedDishesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleRemovedDishesFilter)
{
    ui->setupUi(this);
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
