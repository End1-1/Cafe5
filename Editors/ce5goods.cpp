#include "ce5goods.h"
#include "ui_ce5goods.h"
#include "c5cache.h"

CE5Goods::CE5Goods(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(dbParams, ui->leUnitName, cache_goods_unit);
}

CE5Goods::~CE5Goods()
{
    delete ui;
}

QString CE5Goods::title()
{
    return tr("Goods");
}

QString CE5Goods::table()
{
    return "c_goods";
}
