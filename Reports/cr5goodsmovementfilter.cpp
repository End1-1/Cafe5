#include "cr5goodsmovementfilter.h"
#include "ui_cr5goodsmovementfilter.h"
#include "c5cache.h"

CR5GoodsMovementFilter::CR5GoodsMovementFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5GoodsMovementFilter)
{
    ui->setupUi(this);
    ui->leDocType->setSelector(ui->leDocTypeName, cache_doc_type).setMultiselection(true);
    ui->leGroup->setSelector(ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leGoods->setSelector(ui->leGoodsName, cache_goods, 1, 3).setMultiselection(true);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leReason->setSelector(ui->leReasonName, cache_store_reason).setMultiselection(true);
    ui->leStoreIn->setSelector(ui->leStoreNameIn, cache_goods_store).setMultiselection(true);
    ui->leStoreOut->setSelector(ui->leStoreNameOut, cache_goods_store).setMultiselection(true);
    ui->leInOut->setSelector(ui->leInOutName, cache_store_inout);
    ui->leDocState->setSelector(ui->leDocStateName, cache_doc_state);
    ui->leSupplier->setSelector(ui->leSupplierName, cache_goods_partners);
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
    if (!ui->leDocState->isEmpty()) {
        cond += " and a.f_state in (" + ui->leDocState->text() + ") ";
    }
    in(cond, "ai.f_storein", ui->leStoreIn);
    in(cond, "ai.f_storeout", ui->leStoreOut);
    in(cond, "g.f_acc", ui->leAccount);
    in(cond, "a.f_partner", ui->leSupplier);
    return cond;
}

QString CR5GoodsMovementFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leDocTypeName);
    inFilterText(s, ui->leDocStateName);
    inFilterText(s, ui->leSupplierName);
    inFilterText(s, ui->leReasonName);
    inFilterText(s, ui->leInOutName);
    inFilterText(s, ui->leStoreName);
    inFilterText(s, ui->leStoreNameIn);
    inFilterText(s, ui->leStoreNameOut);
    inFilterText(s, ui->leGoodsName);
    inFilterText(s, ui->leGroupName);
    if (!ui->leAccount->isEmpty()) {
        s += ", " + ui->leAccount->text();
    }
    return s;
}

void CR5GoodsMovementFilter::setDate(const QDate &d1, const QDate &d2)
{
    ui->deStart->setDate(d1);
    ui->deEnd->setDate(d2);
    saveFilter(this);
}

void CR5GoodsMovementFilter::setDocType(const QString &docType)
{
    ui->leDocType->setValue(docType);
    saveFilter(this);
}

void CR5GoodsMovementFilter::setStore(const QString &store)
{
    ui->leStore->setValue(store);
    saveFilter(this);
}

void CR5GoodsMovementFilter::setGoods(const QString &goods)
{
    ui->leGoods->setValue(goods);
    saveFilter(this);
}

void CR5GoodsMovementFilter::setReason(const QString &reason)
{
    ui->leReason->setText(reason);
    saveFilter(this);
}

void CR5GoodsMovementFilter::setInOut(int inout)
{
    ui->leInOut->setValue(inout);
    saveFilter(this);
}

QString CR5GoodsMovementFilter::date1s() const
{
    return ui->deStart->toMySQLDate();
}

QString CR5GoodsMovementFilter::date2s() const
{
    return ui->deEnd->toMySQLDate();
}

QString CR5GoodsMovementFilter::partners() const
{
    return ui->leSupplier->text();
}
