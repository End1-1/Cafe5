#include "cr5dishpriceselfcostfilter.h"
#include "ui_cr5dishpriceselfcostfilter.h"
#include "c5cache.h"
#include <QSettings>

CR5DishPriceSelfCostFilter::CR5DishPriceSelfCostFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5DishPriceSelfCostFilter)
{
    ui->setupUi(this);
    ui->leMenu->setSelector(dbParams, ui->leMenuName, cache_menu_names);
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

bool CR5DishPriceSelfCostFilter::isSimpleView()
{
    return ui->rbSimpleView->isChecked();
}

QString CR5DishPriceSelfCostFilter::menuId()
{
    return ui->leMenu->text();
}
