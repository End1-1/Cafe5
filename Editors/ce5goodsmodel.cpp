#include "ce5goodsmodel.h"
#include "ui_ce5goodsmodel.h"

CE5GoodsModel::CE5GoodsModel(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5GoodsModel)
{
    ui->setupUi(this);
}

CE5GoodsModel::~CE5GoodsModel()
{
    delete ui;
}

QString CE5GoodsModel::title()
{
    return tr("Model");
}

QString CE5GoodsModel::table()
{
    return "c_goods_model";
}
