#include "cr5saleandstorefilter.h"
#include "ui_cr5saleandstorefilter.h"
#include "c5cache.h"

CR5SaleAndStoreFilter::CR5SaleAndStoreFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5SaleAndStoreFilter)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leGoods->setSelector(ui->leGoodsName, cache_goods, 1, 3).setMultiselection(true);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store).setMultiselection(true);
}

CR5SaleAndStoreFilter::~CR5SaleAndStoreFilter()
{
    delete ui;
}

QString CR5SaleAndStoreFilter::condition()
{
    return "";
}

QString CR5SaleAndStoreFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leStoreName);
    inFilterText(s, ui->leGroupName);
    inFilterText(s, ui->leGoodsName);
    return s;
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
