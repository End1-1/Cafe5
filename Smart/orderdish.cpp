#include "orderdish.h"
#include "ui_orderdish.h"
#include "c5utils.h"
#include "dish.h"
#include <QStyle>

OrderDish::OrderDish(const Dish &d, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderDish),
    fDish(d)
{
    ui->setupUi(this);
    updateInfo();
}

OrderDish::~OrderDish()
{
    delete ui;
}

void OrderDish::updateInfo()
{
    ui->lbOrderDishName->setText(fDish.name);
    ui->lbOrderQty->setText(float_str(fDish.qty, 2));
    ui->lbOrderPrice->setText(float_str(fDish.price, 2));
    ui->lbOrderAmount->setText(float_str(fDish.qty * fDish.price, 2));
}

void OrderDish::setSelected(bool s)
{
    ui->orderDishWidget->setProperty("selected", s);
    ui->lbOrderQty->setProperty("selected", s);
    ui->lbOrderDishName->setProperty("selected", s);
    ui->lbOrderPrice->setProperty("selected", s);
    ui->lbOrderAmount->setProperty("selected", s);
    ui->orderDishWidget->style()->polish(ui->orderDishWidget);
    ui->lbOrderQty->style()->polish(ui->lbOrderQty);
    ui->lbOrderDishName->style()->polish(ui->lbOrderDishName);
    ui->lbOrderPrice->style()->polish(ui->lbOrderPrice);
    ui->lbOrderAmount->style()->polish(ui->lbOrderAmount);
}
