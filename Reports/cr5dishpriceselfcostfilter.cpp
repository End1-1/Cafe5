#include "cr5dishpriceselfcostfilter.h"
#include "ui_cr5dishpriceselfcostfilter.h"
#include "c5cache.h"
#include <QSettings>

CR5DishPriceSelfCostFilter::CR5DishPriceSelfCostFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5DishPriceSelfCostFilter)
{
    ui->setupUi(this);
    ui->leMenu->setSelector(ui->leMenuName, cache_menu_names);
    ui->leGoods->setSelector(ui->leGoodsName, cache_goods, 1, 3).setMultiselection(true);
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    ui->rbSimpleView->setChecked(s.value("ViewMode").toBool());
    ui->rbExtendedView->setChecked(!s.value("ViewMode").toBool());
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

void CR5DishPriceSelfCostFilter::saveFilter(QWidget *parent)
{
    C5FilterWidget::saveFilter(parent);
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    s.setValue("ViewMode", ui->rbSimpleView->isChecked());
}

void CR5DishPriceSelfCostFilter::restoreFilter(QWidget *parent)
{
    C5FilterWidget::restoreFilter(parent);
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    ui->rbSimpleView->setChecked(s.value("ViewMode").toBool());
}

int CR5DishPriceSelfCostFilter::viewMode()
{
    if (ui->rbSimpleView->isChecked()) {
        return 1;
    }
    if (ui->rbExtendedView->isChecked()) {
        return 2;
    }
    if (ui->rbExtendedView_2->isChecked()) {
        return 3;
    }
    return 0;
}

QString CR5DishPriceSelfCostFilter::menuId()
{
    return ui->leMenu->text();
}

bool CR5DishPriceSelfCostFilter::baseOnSale()
{
    return ui->chSaleDate->isChecked();
}

const QDate CR5DishPriceSelfCostFilter::d1()
{
    return ui->de1->date();
}

const QDate CR5DishPriceSelfCostFilter::d2()
{
    return ui->de2->date();
}

int CR5DishPriceSelfCostFilter::menuState()
{
    if (ui->rbAllState->isChecked()) {
        return 2;
    }
    if (ui->rbInactiveitems->isChecked()) {
        return 0;
    }
    if (ui->rbActiveState->isChecked()) {
        return 1;
    }
    return -1;
}

QString CR5DishPriceSelfCostFilter::goods()
{
    return ui->leGoods->text();
}
