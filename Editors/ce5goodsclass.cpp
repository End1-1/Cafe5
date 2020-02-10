#include "ce5goodsclass.h"
#include "ui_ce5goodsclass.h"

CE5GoodsClass::CE5GoodsClass(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5GoodsClass)
{
    ui->setupUi(this);
}

CE5GoodsClass::~CE5GoodsClass()
{
    delete ui;
}

QString CE5GoodsClass::title()
{
    return tr("Goods class");
}

QString CE5GoodsClass::table()
{
    return "c_goods_classes";
}
