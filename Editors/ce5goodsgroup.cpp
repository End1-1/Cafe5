#include "ce5goodsgroup.h"
#include "ui_ce5goodsgroup.h"

CE5GoodsGroup::CE5GoodsGroup(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5GoodsGroup)
{
    ui->setupUi(this);
}

CE5GoodsGroup::~CE5GoodsGroup()
{
    delete ui;
}
