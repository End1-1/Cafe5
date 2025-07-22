#include "cr5goodsqtyreminderfilter.h"
#include "ui_cr5goodsqtyreminderfilter.h"
#include "c5cache.h"

CR5GoodsQtyReminderFilter::CR5GoodsQtyReminderFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5GoodsQtyReminderFilter)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(ui->leGroupname, cache_goods_group);
    ui->leMaterial->setSelector(ui->leMaterialName, cache_goods);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store);
    ui->leUnit->setSelector(ui->leUnitName, cache_goods_unit);
}

CR5GoodsQtyReminderFilter::~CR5GoodsQtyReminderFilter()
{
    delete ui;
}

QString CR5GoodsQtyReminderFilter::condition()
{
    QString cond = " ";
    in(cond, "a.f_store", ui->leStore);
    in(cond, "g.f_group", ui->leGroup);
    in(cond, "g.f_id", ui->leMaterial);
    in(cond, "g.f_unit", ui->leUnit);
    return cond;
}

QString CR5GoodsQtyReminderFilter::filterText()
{
    QString s;
    inFilterText(s, ui->leStoreName);
    inFilterText(s, ui->leMaterialName);
    inFilterText(s, ui->leUnitName);
    inFilterText(s, ui->leGroupname);
    return s;
}
