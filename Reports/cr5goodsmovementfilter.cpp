#include "cr5goodsmovementfilter.h"
#include "ui_cr5goodsmovementfilter.h"
#include "c5cache.h"

CR5GoodsMovementFilter::CR5GoodsMovementFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5GoodsMovementFilter)
{
    ui->setupUi(this);
    ui->leGoods->setSelector(fDBParams, ui->leGoodsName, cache_goods, 1, 3);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
    ui->leInOut->setSelector(fDBParams, ui->leInOutName, cache_store_inout);
}

CR5GoodsMovementFilter::~CR5GoodsMovementFilter()
{
    delete ui;
}

QString CR5GoodsMovementFilter::condition()
{
    QString cond;
    cond += " a.f_date between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate();
    if (!ui->leInOut->isEmpty()) {
        cond += " and s.f_type in (" + ui->leInOut->text() + ") ";
    }
    if (!ui->leStore->isEmpty()) {
        cond += " and s.f_store in (" + ui->leStore->text() + ") ";
    }
    if (!ui->leGoods->isEmpty()) {
        cond += " and s.f_goods in (" + ui->leGoods->text() + ") ";
    }
    return cond;
}
