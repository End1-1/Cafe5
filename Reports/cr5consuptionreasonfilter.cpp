#include "cr5consuptionreasonfilter.h"
#include "ui_cr5consuptionreasonfilter.h"
#include "c5cache.h"

CR5ConsuptionReasonFilter::CR5ConsuptionReasonFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ConsuptionReasonFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
    ui->leDishPart->setSelector(dbParams, ui->leDishPartName, cache_dish_part2);
    ui->leDish->setSelector(dbParams, ui->leDishName, cache_dish);
    ui->leGoods->setSelector(dbParams, ui->leGoodsName, cache_goods);
}

CR5ConsuptionReasonFilter::~CR5ConsuptionReasonFilter()
{
    delete ui;
}

QString CR5ConsuptionReasonFilter::condition()
{
    QString cond = QString(" oh.f_datecash between %1 and %2 ")
            .arg(ui->deStart->toMySQLDate())
            .arg(ui->deEnd->toMySQLDate());
    if (!ui->leStore->isEmpty()) {
        cond += " and og.f_store in (" + ui->leStore->text() + ") ";
    }
    if (!ui->leDishPart->isEmpty()) {
        cond += " and dp.f_id in ("  + ui->leDishPart->text() + ") ";
    }
    if (!ui->leDish->isEmpty()) {
        cond += " and ob.f_dish in (" + ui->leDish->text() + ") ";
    }
    if (!ui->leGoods->isEmpty()) {
        cond += " and og.f_goods in (" + ui->leGoods->text() + ") ";
    }
    return cond;
}

void CR5ConsuptionReasonFilter::setDateFilter(const QDate &d1, const QDate &d2)
{
    ui->deStart->setDate(d1);
    ui->deEnd->setDate(d2);
}

void CR5ConsuptionReasonFilter::setStoreFilter(int store)
{
    ui->leStore->setValue(store);
}

void CR5ConsuptionReasonFilter::setGoodsFilter(int goods)
{
    ui->leGoods->setValue(goods);
}