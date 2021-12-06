#include "cr5draftoutputbyrecipefilter.h"
#include "ui_cr5draftoutputbyrecipefilter.h"
#include "c5cache.h"
#include "c5utils.h"

CR5DraftOutputByRecipeFilter::CR5DraftOutputByRecipeFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5DraftOutputByRecipeFilter)
{
    ui->setupUi(this);
    ui->leDishState->setSelector(dbParams, ui->leDishStateName, cache_dish_state).setMultiselection(true);
    ui->leDishState->setValue(DISH_STATE_OK);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2).setMultiselection(true);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leGoodsGroup->setSelector(dbParams, ui->leGoodsGroupName, cache_goods_group).setMultiselection(true);
}

CR5DraftOutputByRecipeFilter::~CR5DraftOutputByRecipeFilter()
{
    delete ui;
}

QString CR5DraftOutputByRecipeFilter::condition()
{
    QString cond = QString(" oh.f_datecash between '%1' and '%2'")
            .arg(ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL))
            .arg(ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    if (ui->leDishState->isEmpty()) {
        cond += QString(" and ob.f_state in(%1,%2)")
                .arg(DISH_STATE_OK)
                .arg(DISH_STATE_VOID);
    } else {
        in(cond, "ob.f_state", ui->leDishState);
    }
    in(cond, "dp.f_id", ui->lePart2);
    in(cond, "gr.f_id", ui->leGoodsGroup);
    in(cond, "st.f_id", ui->leStore);
    return cond;
}

void CR5DraftOutputByRecipeFilter::addDays(int day)
{
    ui->deStart->setDate(ui->deStart->date().addDays(day));
    ui->deEnd->setDate(ui->deEnd->date().addDays(day));
}

QDate CR5DraftOutputByRecipeFilter::date1()
{
    return ui->deStart->date();
}

QDate CR5DraftOutputByRecipeFilter::date2()
{
    return ui->deEnd->date();
}

int CR5DraftOutputByRecipeFilter::store()
{
    return ui->leStore->getInteger();
}
