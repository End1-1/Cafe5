#include "menudish.h"
#include "ui_menudish.h"
#include "c5utils.h"
#include "dish.h"

MenuDish::MenuDish(Dish *d, bool groupVisible) :
    QWidget(nullptr),
    ui(new Ui::menudish),
    fDish(d)
{
    ui->setupUi(this);
    ui->lbCalcWarning->setVisible(false);
    ui->lbSelfcostWarning->setVisible(false);
    ui->lbDishPrice->setText(float_str(d->price, 2));
    ui->lbDishName->setText(d->name);
    ui->lbCalcWarning->setVisible(d->netWeight < 0.00001);
    ui->lbSelfcostWarning->setVisible(d->price <= d->cost);
    ui->lbDishGroupName->setText(d->typeName);
    ui->lbDishGroupName->setVisible(groupVisible);
}

MenuDish::~MenuDish()
{
    delete ui;
}
