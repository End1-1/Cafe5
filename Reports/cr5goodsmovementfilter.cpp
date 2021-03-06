#include "cr5goodsmovementfilter.h"
#include "ui_cr5goodsmovementfilter.h"
#include "c5cache.h"

CR5GoodsMovementFilter::CR5GoodsMovementFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5GoodsMovementFilter)
{
    ui->setupUi(this);
    ui->leDocType->setSelector(fDBParams, ui->leDocTypeName, cache_doc_type).setMultiselection(true);
    ui->leGroup->setSelector(fDBParams, ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leGoods->setSelector(fDBParams, ui->leGoodsName, cache_goods, 1, 3).setMultiselection(true);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leReason->setSelector(fDBParams, ui->leReasonName, cache_store_reason).setMultiselection(true);
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
    if (!ui->leGroup->isEmpty()) {
        cond += " and g.f_group in (" + ui->leGroup->text() + ") ";
    }
    if (!ui->leDocType->isEmpty()) {
        cond += " and a.f_type in (" + ui->leDocType->text() + ") ";
    }
    if (!ui->leGoods->isEmpty()) {
        cond += " and s.f_goods in (" + ui->leGoods->text() + ") ";
    }
    if (!ui->leReason->isEmpty()) {
        cond += " and s.f_reason in (" + ui->leReason->text() + ") ";
    }
    return cond;
}
