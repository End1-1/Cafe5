#include "ce5dishpart2.h"
#include "ui_ce5dishpart2.h"
#include "c5cache.h"
#include "ce5dishpart1.h"
#include <QColorDialog>

CE5DishPart2::CE5DishPart2(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPart2)
{
    ui->setupUi(this);
    ui->leDept->setSelector(dbParams, ui->leDeptName, cache_dish_part1);
    ui->leParent->setSelector(dbParams, ui->leParentName, cache_dish_part2);
    ui->lePosition->setSelector(dbParams, ui->lePositionName, cache_users_groups);
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

bool CE5DishPart2::checkData(QString &err)
{
    CE5Editor::checkData(err);
    if (ui->leParent->getInteger()) {
        if (ui->leParent->getInteger() == ui->leCode->getInteger()) {
            err += "<br>" + tr("Cannot parent itself");
        }
    }
    return err.isEmpty();
}

void CE5DishPart2::selectColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}

void CE5DishPart2::on_btnNewPart_clicked()
{
    CE5DishPart1 *ep = new CE5DishPart1(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leDept->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}
