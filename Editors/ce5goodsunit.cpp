#include "ce5goodsunit.h"
#include "ui_ce5goodsunit.h"
#include <QDoubleValidator>

CE5GoodsUnit::CE5GoodsUnit(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5GoodsUnit)
{
    ui->setupUi(this);
    ui->leDefaultQty->setValidator(new QDoubleValidator(0,99,3));
}

CE5GoodsUnit::~CE5GoodsUnit()
{
    delete ui;
}

QString CE5GoodsUnit::title()
{
    return tr("Goods unit");
}

QString CE5GoodsUnit::table()
{
    return "c_units";
}
