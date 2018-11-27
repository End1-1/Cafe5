#include "ce5dishpart1.h"
#include "ui_ce5dishpart1.h"

CE5DishPart1::CE5DishPart1(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPart1)
{
    ui->setupUi(this);
}

CE5DishPart1::~CE5DishPart1()
{
    delete ui;
}

QString CE5DishPart1::title()
{
    return tr("Dish department");
}

QString CE5DishPart1::table()
{
    return "d_part1";
}
