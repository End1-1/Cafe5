#include "ce5goodsgroup.h"
#include "ui_ce5goodsgroup.h"
#include "c5cache.h"
#include <QColorDialog>

CE5GoodsGroup::CE5GoodsGroup(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5GoodsGroup)
{
    ui->setupUi(this);
    ui->leParentGroup->setSelector(ui->leParentGroupName, cache_goods_group);
    connect(ui->leColor, &C5LineEditWithSelector::doubleClicked, this, &CE5GoodsGroup::setColor);
}

CE5GoodsGroup::~CE5GoodsGroup()
{
    delete ui;
}

void CE5GoodsGroup::setColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}
