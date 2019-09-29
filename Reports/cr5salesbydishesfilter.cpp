#include "cr5salesbydishesfilter.h"
#include "ui_cr5salesbydishesfilter.h"
#include "c5cache.h"

CR5SalesByDishesFilter::CR5SalesByDishesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SalesByDishesFilter)
{
    ui->setupUi(this);
    ui->leDishState->setSelector(dbParams, ui->leStateName, cache_dish_state);
    ui->leDishState->setValue(DISH_STATE_OK);
    ui->lePart1->setSelector(dbParams, ui->lePart1Name, cache_dish_part1);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
}

CR5SalesByDishesFilter::~CR5SalesByDishesFilter()
{
    delete ui;
}

QString CR5SalesByDishesFilter::condition()
{
    QString result = QString(" oh.f_datecash between '%1' and '%2' ").arg(ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL)).arg(ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    if (!ui->leDishState->isEmpty()) {
        result += QString(" and ob.f_state in (%1) ").arg(ui->leDishState->text());
    }
    if (!ui->lePart1->isEmpty()) {
        result += QString(" and dpd.f_id in (%1) ").arg(ui->lePart1->text());
    }
    if (!ui->lePart2->isEmpty()) {
        result += QString(" and d.f_part in (%1) ").arg(ui->lePart2->text());
    }
    if (!ui->leStore->isEmpty()) {
        result += QString(" and ob.f_store in (%1) ").arg(ui->leStore->text());
    }
    return result;
}
