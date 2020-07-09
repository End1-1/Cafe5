#include "cr5saleandstorefilter.h"
#include "ui_cr5saleandstorefilter.h"
#include "c5cache.h"

CR5SaleAndStoreFilter::CR5SaleAndStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleAndStoreFilter)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(fDBParams, ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leGoods->setSelector(fDBParams, ui->leGoodsName, cache_goods, 1, 3).setMultiselection(true);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
}

CR5SaleAndStoreFilter::~CR5SaleAndStoreFilter()
{
    delete ui;
}

QString CR5SaleAndStoreFilter::condition()
{
    return "";
}

const QDate CR5SaleAndStoreFilter::date1() const
{
    return ui->deStart->date();
}

const QDate CR5SaleAndStoreFilter::date2() const
{
    return ui->deEnd->date();
}

const QString CR5SaleAndStoreFilter::store() const
{
    return ui->leStore->text();
}

const QString CR5SaleAndStoreFilter::goodsGroup() const
{
    return ui->leGroup->text();
}

const QString CR5SaleAndStoreFilter::goods() const
{
    return ui->leGoods->text();
}

CR5SaleAndStoreFilter::Display CR5SaleAndStoreFilter::display()
{
    if (ui->rbDisplayGoods->isChecked()) {
        return dGoods;
    }
    if (ui->rbDisplayGroups->isChecked()) {
        return dGroups;
    }
    return dNone;
}
