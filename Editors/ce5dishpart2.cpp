#include "ce5dishpart2.h"
#include "ui_ce5dishpart2.h"
#include "c5cache.h"
#include <QColorDialog>

CE5DishPart2::CE5DishPart2(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPart2)
{
    ui->setupUi(this);
    ui->leDept->setSelector(dbParams, ui->leDeptName, cache_dish_part1);
    connect(ui->leColor, SIGNAL(doubleClicked()), this, SLOT(selectColor()));
}

CE5DishPart2::~CE5DishPart2()
{
    delete ui;
}

QString CE5DishPart2::title()
{
    return tr("Dish type");
}

QString CE5DishPart2::table()
{
    return "d_part2";
}

void CE5DishPart2::selectColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}
