#include "cr5menureviewfilter.h"
#include "ui_cr5menureviewfilter.h"
#include "c5cache.h"

CR5MenuReviewFilter::CR5MenuReviewFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MenuReviewFilter)
{
    ui->setupUi(this);
    ui->leMenu->setSelector(dbParams, ui->leMenuName, cache_menu_names);
    ui->leDept->setSelector(dbParams, ui->leDeptName, cache_dish_part1);
    ui->leType->setSelector(dbParams, ui->leTypeName, cache_dish_part2, 1, 2).setMultiselection(true);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_dish_menu_state);
}

CR5MenuReviewFilter::~CR5MenuReviewFilter()
{
    delete ui;
}

QString CR5MenuReviewFilter::condition()
{
    QString cond = "where m.f_id>0 ";
    if (!ui->leMenu->isEmpty()) {
        cond += " and m.f_menu in (" + ui->leMenu->text() + ") ";
    }
    if (!ui->leDept->isEmpty()) {
        cond += " and dp1.f_id in (" + ui->leDept->text() + ") ";
    }
    if (!ui->leType->isEmpty()) {
        cond += " and dp2.f_id in (" + ui->leType->text() + ") ";
    }
    if (!ui->leStore->isEmpty()) {
        cond += " and m.f_store in (" + ui->leStore->text() + ") ";
    }
    if (!ui->leState->isEmpty()) {
        cond += " and m.f_state in (" + ui->leState->text() + ") ";
    }
    return cond;
}
