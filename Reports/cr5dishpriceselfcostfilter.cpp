#include "cr5dishpriceselfcostfilter.h"
#include "ui_cr5dishpriceselfcostfilter.h"
#include "c5cache.h"

CR5DishPriceSelfCostFilter::CR5DishPriceSelfCostFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5DishPriceSelfCostFilter)
{
    ui->setupUi(this);
    ui->leMenu->setSelector(dbParams, ui->leMenuName, cache_menu_names);
}

CR5DishPriceSelfCostFilter::~CR5DishPriceSelfCostFilter()
{
    delete ui;
}

QString CR5DishPriceSelfCostFilter::condition()
{
    if (ui->leMenu->isEmpty()) {
        return "";
    } else {
        return " m.f_menu=" + ui->leMenu->text();
    }
}
