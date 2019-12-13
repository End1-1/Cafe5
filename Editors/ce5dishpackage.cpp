#include "ce5dishpackage.h"
#include "ui_ce5dishpackage.h"

CE5DishPackage::CE5DishPackage(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPackage)
{
    ui->setupUi(this);
}

CE5DishPackage::~CE5DishPackage()
{
    delete ui;
}

QString CE5DishPackage::title()
{
    return tr("Dish package");
}

QString CE5DishPackage::table()
{
    return "d_package";
}
